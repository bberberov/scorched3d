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

#include <weapons/WeaponAimedUnder.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroupEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankLib.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <common/Defines.h>
#include <list>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponAimedUnder);

WeaponAimedUnder::WeaponAimedUnder() : 
	moveUnderground_(true)
{
}

WeaponAimedUnder::~WeaponAimedUnder()
{
}

bool WeaponAimedUnder::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!WeaponAimed::parseXML(context, accessoryNode)) return false;

	// Get optional moveunderground attribute
	accessoryNode->getNamedChild("moveunderground", moveUnderground_, false);

	return true;
}

void WeaponAimedUnder::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &sentPosition, FixedVector &oldvelocity)
{
	FixedVector position = sentPosition;
	if (moveUnderground_)
	{
		fixed height = context.getLandscapeMaps().getGroundMaps().
			getInterpHeight(position[0], position[1]);
		if (position[2] < height + 1)
		{
			position[2] = context.getLandscapeMaps().getGroundMaps().
				getInterpHeight(position[0], position[1]) / 2;
		}
	}

	fireAimedWeapon(context, weaponContext, position, false);
}

void WeaponAimedUnder::aimShot(ScorchedContext &context,
	RandomGenerator &random,
	FixedVector &position, FixedVector &shootAt,
	fixed &angleXYDegs, fixed &angleYZDegs, fixed &power)
{
	TankLib::getSniperShotTowardsPosition(
		context,
		position, 
		shootAt, 
		-1, 
		angleXYDegs, 
		angleYZDegs, 
		power);
}
