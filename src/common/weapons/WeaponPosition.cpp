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

#include <weapons/WeaponPosition.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>

REGISTER_ACCESSORY_SOURCE(WeaponPosition);

WeaponPosition::WeaponPosition() :
	positionOffset_(0, 0, 0),
	aimedWeapon_(0)
{

}

WeaponPosition::~WeaponPosition()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponPosition::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode) 
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

	if (!accessoryNode->getNamedChild("position", position_)) return false;

	// Optional random offset
	accessoryNode->getNamedChild("positionoffset", positionOffset_, false);

	// Force it to ground level
	accessoryNode->getNamedChild("onground", onGround_, false);

	return true;
}

void WeaponPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Position the fired weapon at the desired position, 
	// +/- optional random offset
	RandomGenerator &random = context.getSimulator().getRandomGenerator();

	if(onGround_)
	{
		fixed minHeight = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
		                position[0], position[1]);
		position_[2] = minHeight;
	}

	FixedVector newPosition;
	newPosition[0] = position_[0] - positionOffset_[0] +
		positionOffset_[0] * 2 * random.getRandFixed("WeaponPosition");
	newPosition[1] = position_[1] - positionOffset_[1] +
		positionOffset_[1] * 2 * random.getRandFixed("WeaponPosition");
	newPosition[2] = position_[2] - positionOffset_[2] +		
		positionOffset_[2] * 2 * random.getRandFixed("WeaponPosition");
	
	aimedWeapon_->fire(context, weaponContext, newPosition, velocity);

}

