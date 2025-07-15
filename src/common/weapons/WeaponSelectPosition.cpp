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

#include <weapons/WeaponSelectPosition.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetContainer.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <landscapemap/LandscapeMaps.h>

REGISTER_ACCESSORY_SOURCE(WeaponSelectPosition);

WeaponSelectPosition::WeaponSelectPosition() :
	aimedWeapon_(0)
{

}

WeaponSelectPosition::~WeaponSelectPosition()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponSelectPosition::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
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

	return true;
}

void WeaponSelectPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (tanket && tanket->getAlive())
	{
		FixedVector newPosition;
		newPosition[0] = fixed(weaponContext.getInternalContext().getSelectPositionX());
		newPosition[1] = fixed(weaponContext.getInternalContext().getSelectPositionY());
		newPosition[2] = context.getLandscapeMaps().getGroundMaps().getHeight(
			weaponContext.getInternalContext().getSelectPositionX(),
			weaponContext.getInternalContext().getSelectPositionY());

		aimedWeapon_->fire(context, weaponContext, newPosition, velocity);
	}
}

