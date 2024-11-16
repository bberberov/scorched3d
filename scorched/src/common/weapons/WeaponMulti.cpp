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

#include <weapons/WeaponMulti.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponMulti);

WeaponMulti::WeaponMulti()
{

}

WeaponMulti::~WeaponMulti()
{
	while (!subWeapons_.empty())
	{
		delete subWeapons_.back();
		subWeapons_.pop_back();
	}
}

bool WeaponMulti::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	for (int i=1;;i++)
	{
		// Get the next weapon
		char buffer[128];
		snprintf(buffer, 128, "subweapon%i", i);
		XMLNode *subNode = 0;
		accessoryNode->getNamedChild(buffer, subNode, false);
		if (!subNode) break;
		
		// Check next weapon is correct type
		AccessoryPart *accessory = context.getAccessoryStore().
			createAccessoryPart(context, parent_, subNode);
		if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
		{
			return subNode->returnError("Failed to find sub weapon, not a weapon");
		}
		subWeapons_.push_back((Weapon*) accessory);
	}

	return true;
}

void WeaponMulti::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	std::list<Weapon *>::iterator itor;
	for (itor = subWeapons_.begin();
		 itor != subWeapons_.end();
		 ++itor)
	{
		Weapon *weapon = *itor;
		weapon->fire(context, weaponContext, position, velocity);
	}
}
