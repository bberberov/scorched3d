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

#include <weapons/WeaponAimedOver.h>
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
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponAimedOver);

WeaponAimedOver::WeaponAimedOver() 
{

}

WeaponAimedOver::~WeaponAimedOver()
{
}

bool WeaponAimedOver::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!WeaponAimed::parseXML(context, accessoryNode)) return false;
	return true;
}

void WeaponAimedOver::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &sentPosition, FixedVector &oldvelocity)
{
	FixedVector position = sentPosition;

	// Make sure that this position is above ground
	fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		position[0], position[1]);
	if (position[2] < minHeight + fixed(true, 5000))
	{
		position[2] = minHeight + fixed(true, 5000);
	}

	bool ceiling = false;
	{
		// This will return MAX_FLT when there is no roof
		fixed maxHeight = context.getLandscapeMaps().getRoofMaps().getInterpRoofHeight(
			position[0], position[1]);
		if (position[2] > maxHeight - 1)
		{
			ceiling = true;
			position[2] = maxHeight - 1;
		}
	}

	fireAimedWeapon(context, weaponContext, position, ceiling);
}

void WeaponAimedOver::aimShot(ScorchedContext &context,
	RandomGenerator &random,
	FixedVector &position, FixedVector &shootAt,
	fixed &angleXYDegs, fixed &angleYZDegs, fixed &power)
{
	TankLib::getShotTowardsPosition(
		context,
		random,
		position, 
		shootAt, 
		angleXYDegs, 
		angleYZDegs, 
		power);
	power *= fixed(true, 6000);
}
