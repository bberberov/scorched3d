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

#include <weapons/WeaponTankPosition.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetContainer.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <tank/TankLib.h>

REGISTER_ACCESSORY_SOURCE(WeaponTankPosition);

WeaponTankPosition::WeaponTankPosition() : 
	sightPos_(false), aimedWeapon_(0)
{

}

WeaponTankPosition::~WeaponTankPosition()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponTankPosition::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
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

	accessoryNode->getNamedChild("sightpos", sightPos_, false);

	return true;
}

void WeaponTankPosition::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// This weapon re-centers the current shot on the firing tank.
	// This can be used for firing from a moving tank

	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (tanket && tanket->getAlive())
	{
		FixedVector newPosition = tanket->getLife().getTargetPosition();
		if (sightPos_)
		{
			newPosition = tanket->getLife().getTankTurretPosition();
		}

		aimedWeapon_->fire(context, weaponContext, newPosition, velocity);
	}
}

