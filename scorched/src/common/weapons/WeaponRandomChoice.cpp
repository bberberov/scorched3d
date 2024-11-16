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

#include <weapons/WeaponRandomChoice.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <common/Defines.h>
#include <stdlib.h>

REGISTER_ACCESSORY_SOURCE(WeaponRandomChoice);

WeaponRandomChoice::WeaponRandomChoice() : 
	totalWeight_(0)
{
}

WeaponRandomChoice::~WeaponRandomChoice()
{
	while (!weaponsChoice_.empty())
	{
		delete weaponsChoice_.back().weapon;
		weaponsChoice_.pop_back();
	}
}

bool WeaponRandomChoice::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	for (int i=1;;i++)
	{
		// Get the next weaponchoice
		char buffer[128];
		snprintf(buffer, 128, "weaponchoice%i", i);
		XMLNode *subNode = 0;
		accessoryNode->getNamedChild(buffer, subNode, false);
		if (!subNode) break;

		// Get the weight
		int weight = 0;
		if (!subNode->getNamedChild("weight", weight)) return false;

		// Get the weapon
		XMLNode *weaponNode = 0;
		if (!subNode->getNamedChild("weapon", weaponNode)) return false;
		AccessoryPart *accessory = context.getAccessoryStore().
			createAccessoryPart(context, parent_, weaponNode);
		if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
		{
			return weaponNode->returnError("Failed to find sub weapon, not a weapon");
		}
		Weapon *weapon = (Weapon*) accessory;

		// Add this entry
		WeaponWeight weaponWeight = { weight, weapon };
		weaponsChoice_.push_back(weaponWeight);
		totalWeight_ += weight;

		// Check the node is empty
		if (!subNode->failChildren()) return false;
	}

	if (weaponsChoice_.empty())
	{
		return accessoryNode->returnError(
			"Must provide at least one random choice");
	}

	return accessoryNode->failChildren();
}

void WeaponRandomChoice::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	unsigned int randWeight = random.getRandUInt("WeaponRandomChoice") % totalWeight_;
	unsigned int currentWeight = 0;

	std::list<WeaponWeight>::iterator itor;
	for (itor = weaponsChoice_.begin();
		itor != weaponsChoice_.end();
		++itor)
	{
		WeaponWeight &weightEntry = *itor;
		currentWeight += weightEntry.weight;

		if (currentWeight >= randWeight)
		{
			Weapon *weapon = weightEntry.weapon;
			weapon->fire(context, weaponContext, position, velocity);
			break;
		}
	}
}
