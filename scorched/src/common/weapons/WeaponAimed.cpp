////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <weapons/WeaponAimed.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankLib.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <list>
#include <map>
#include <math.h>

WeaponAimed::WeaponAimed() :
	warHeads_(0),
	aimedWeapon_(0),
	randomWhenNoTargets_(true),
	noSelfHoming_(false),
	maxAimedDistance_("WeaponAimed::maxAimedDistance"),
	percentageMissChance_("WeaponAimed::percentageMissChance"),
	maxInacuracy_("WeaponAimed::maxInacuracy")
{

}

WeaponAimed::~WeaponAimed()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponAimed::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("aimedweapon", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore().
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	aimedWeapon_ = (Weapon*) accessory;

	// Get the accessory warheads
	if (!accessoryNode->getNamedChild("nowarheads", warHeads_)) return false;
	
	// Get the accessory aimed distance
	if (!accessoryNode->getNamedChild("maxaimdistance", maxAimedDistance_)) return false;
	
	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("percentagemiss", percentageMissChance_)) return false;

	// Get the accessory percentage miss chance
	if (!accessoryNode->getNamedChild("inaccuracy", maxInacuracy_)) return false;

	// If set only consider items of this group
	accessoryNode->getNamedChild("groupname", groupName_, false);

	// Do we ignore shots to ourself
	accessoryNode->getNamedChild("noselfhoming", noSelfHoming_, false);

	// If there are no items in group/range just randomly shoot
	accessoryNode->getNamedChild("randomwhennotargets", randomWhenNoTargets_, false);

	return true;
}

void WeaponAimed::addWeaponSyncCheck(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponFire %s-%u-%s %u %s %s \"%s\"",
		getParent()->getName(), getParent()->getAccessoryId(), getAccessoryTypeName(),
		weaponContext.getPlayerId(),
		position.asQuickString(), velocity.asQuickString(),
		groupName_.c_str()));
}

void WeaponAimed::fireAimedWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, bool invert)
{
	// Get a list of possible destinations
	std::list<FixedVector *> positions;
	if (groupName_.empty())
	{
		std::map<unsigned int, Tanket *> &allTankets =
			context.getTargetContainer().getTankets();
		std::map<unsigned int, Tanket *>::iterator itor;
		for (itor = allTankets.begin();
			itor != allTankets.end();
			++itor)
		{
			Tanket *tanket = itor->second;
			if (tanket->getAlive())
			{
				if (noSelfHoming_ && weaponContext.getPlayerId() == tanket->getPlayerId()) continue;
				positions.push_back(&itor->second->getLife().getTargetPosition());
			}
		}
	}
	else
	{
		ObjectGroup *objectGroup = weaponContext.getInternalContext().getLocalGroups().getGroup(groupName_.c_str());
		if (!objectGroup) objectGroup = context.getObjectGroups().getGroup(groupName_.c_str());
		if (objectGroup) 
		{
			ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup);
			ObjectGroupEntry *entry;
			while (entry = iterator.getNext())
			{
				positions.push_back(&entry->getPosition());
			}			
		}
	}

	// Iterate 
	fixed totalDist = 0;
	std::list<std::pair<fixed, FixedVector *> > distances;
	{
		fixed maxAimedDistance = maxAimedDistance_.getValue(context);
		std::list<FixedVector *>::iterator itor;
		for (itor = positions.begin();
			itor != positions.end();
			++itor)
		{
			fixed distance = (*(*itor) - position).Magnitude();
			if (distance < maxAimedDistance)
			{
				totalDist += distance;
				distances.push_back(std::pair<fixed, FixedVector *>(distance, *itor));
			}
		}
	}

	// Turn distance into a probablity that we will fire a the tank
	fixed maxDist = 0;
	{
		if (distances.size() <= 1)
		{
			maxDist = totalDist;
		}
		else
		{
			std::list<std::pair<fixed, FixedVector *> >::iterator itor;
			for (itor = distances.begin();
				itor != distances.end();
				++itor)
			{
				maxDist += totalDist - itor->first;
				itor->first = totalDist - itor->first;
			}
		}
	}

	if (context.getOptionsGame().getWeaponSyncCheck())
	{
		std::string buffer("WeaponAimedTargets ");
		std::list<std::pair<fixed, FixedVector *> >::iterator itor;
		for (itor = distances.begin();
			itor != distances.end();
			++itor)
		{
			buffer += S3D::formatStringBuffer("%s %s, ", itor->first.asQuickString(), itor->second->asQuickString());
		}
		context.getSimulator().addSyncCheck(buffer);
	}

	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	
	// Add a percetage that we will not fire at any tank
	maxDist *= (percentageMissChance_.getValue(context) / 100) + 1;

	// For each war head
	for (int i=0; i<warHeads_; i++)
	{
		// Random probablity
		fixed dist = maxDist * random.getRandFixed("WeaponAimed");

		// Find which tank fits this probability
		FixedVector *shootAt = 0;
		fixed distC = 0;
		std::list<std::pair<fixed, FixedVector *> >::iterator itor;
		for (itor =  distances.begin();
			itor !=  distances.end();
			++itor)
		{
			distC += (*itor).first;
			if (dist < distC)
			{
				shootAt = (*itor).second;
				break;
			}
		}			

		// Calcuate the angle for the shot
		fixed angleXYDegs = random.getRandFixed("WeaponAimed") * 360;
		fixed angleYZDegs = random.getRandFixed("WeaponAimed") * 30 + 50;
		fixed power = random.getRandFixed("WeaponAimed") * 300 + 150;
		if (shootAt)
		{
			if (context.getOptionsGame().getWeaponSyncCheck())
			{
				context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponAimed %s,%s,%s %s",
					angleXYDegs.asQuickString(), angleYZDegs.asQuickString(),
					power.asQuickString(),
					shootAt->asQuickString()));
			}

			// We have a tank to aim at
			// Aim a shot towards it
			aimShot(
				context,
				random,
				position, 
				*shootAt, 
				angleXYDegs, 
				angleYZDegs, 
				power);

			angleXYDegs += (random.getRandFixed("WeaponAimed") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
			angleYZDegs += (random.getRandFixed("WeaponAimed") * maxInacuracy_.getValue(context)) - 
				(maxInacuracy_.getValue(context) / 2);
		}
		else if (!randomWhenNoTargets_)
		{
			continue;
		}
		if (invert) angleYZDegs += 180;

		// Create the shot
		FixedVector &velocity = TankLib::getVelocityVector(
			angleXYDegs, angleYZDegs);
		velocity *= power;

		aimedWeapon_->fire(
			context, weaponContext, position, velocity);	
	}
}
