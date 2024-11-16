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

#include <weapons/WeaponScatterPosition.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponScatterPosition);

WeaponScatterPosition::WeaponScatterPosition() :
	aimedWeapon_(0), scatterpercentage_("WeaponScatterPosition::scatterpercentage")
{

}

WeaponScatterPosition::~WeaponScatterPosition()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponScatterPosition::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
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

	if (!accessoryNode->getNamedChild("landonly", landonly_)) return false;
	if (!accessoryNode->getNamedChild("landheight", landheight_)) return false;
	if (!accessoryNode->getNamedChild("scatterpercentage", scatterpercentage_)) return false;

	return true;
}

void WeaponScatterPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &p, FixedVector &velocity)
{
	// Mininum height, if we are grounding
	fixed allowedHeight = 0;
	LandscapeTex &tex = *context.getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
	{
		LandscapeTexBorderWater *water = 
			(LandscapeTexBorderWater *) tex.border;

		allowedHeight = water->height;
	}

	fixed scatterpercentage = scatterpercentage_.getValue(context);

	fixed arenaWidth = fixed(context.getLandscapeMaps().getGroundMaps().getArenaWidth());
	fixed arenaHeight = fixed(context.getLandscapeMaps().getGroundMaps().getArenaHeight());
	fixed scatterWidth = arenaWidth * scatterpercentage / 100;
	fixed scatterHeight = arenaHeight * scatterpercentage / 100;

	FixedVector pos;
	RandomGenerator &random = context.getSimulator().getRandomGenerator();
	bool ok = false;
	while (!ok)
	{
		ok = true;

		pos[0] = p[0] + (random.getRandFixed("WeaponScatterPosition") * scatterWidth) - (scatterWidth / 2);
		pos[1] = p[1] + (random.getRandFixed("WeaponScatterPosition") * scatterHeight) - (scatterHeight / 2);
		pos[2] = p[2];
		if (landheight_)
		{
			pos[2] = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
				pos[0], pos[1]);
		}

		if (landonly_)
		{
			if (pos[2] < allowedHeight)
			{
				ok = false;
				allowedHeight -= fixed(true, 100);
			}
		}
	}

	aimedWeapon_->fire(context, weaponContext, pos, velocity);
}

