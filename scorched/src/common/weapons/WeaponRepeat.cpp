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

#include <weapons/WeaponRepeat.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponRepeat);

WeaponRepeat::WeaponRepeat() : 
	delay_("WeaponRepeat::delay", 0), repeatWeapon_(0)
{

}

WeaponRepeat::~WeaponRepeat()
{
	delete repeatWeapon_;
	repeatWeapon_ = 0;
}

bool WeaponRepeat::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("repeatweapon", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore().
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	repeatWeapon_ = (Weapon*) accessory;

	if (!accessoryNode->getNamedChild("repeat", repeat_)) return false;

	accessoryNode->getNamedChild("delay", delay_, false);

	return true;
}

void WeaponRepeat::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	if (delay_.getValue(context) == 0)
	{
		for (int i=0; i<repeat_; i++)
		{
			repeatWeapon_->fire(context, weaponContext, position, velocity);
		}
	}
	else
	{
		weaponCallback(context, weaponContext, position, velocity, repeat_);
	}
}

void WeaponRepeat::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	repeatWeapon_->fire(context, weaponContext, position, velocity);

	if (userData > 1)
	{
		context.getActionController().addAction(
			new CallbackWeapon("WeaponRepeat", this, delay_.getValue(context), userData - 1, 
				weaponContext, position, velocity));
	}
}
