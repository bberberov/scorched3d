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

#include <weapons/WeaponVelocity.h>
#include <weapons/AccessoryStore.h>

REGISTER_ACCESSORY_SOURCE(WeaponVelocity);

WeaponVelocity::WeaponVelocity() :
	velocityChange_("WeaponVelocity::velocityChange", 0), abs_(false),
	aimedWeapon_(0)
{

}

WeaponVelocity::~WeaponVelocity()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponVelocity::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
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

	if (!accessoryNode->getNamedChild("velocitychange", velocityChange_)) return false;

	accessoryNode->getNamedChild("abs", abs_, false);

	return true;
}

void WeaponVelocity::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Add a shot that will fall where the original was aimed
	// but with altered velocity
	FixedVector newVelocity;
	if (abs_)
	{
		newVelocity = velocity.Normalize() * 50 * velocityChange_.getValue(context); 
		aimedWeapon_->fire(context, weaponContext, position, newVelocity);
	}
	else
	{
		newVelocity = velocity * velocityChange_.getValue(context);
		aimedWeapon_->fire(context, weaponContext, position, newVelocity);
	}
}

