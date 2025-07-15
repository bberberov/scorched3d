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

#include <weapons/WeaponMirv.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponMirv);

WeaponMirv::WeaponMirv() :
	noWarheads_(0), 
	hspreadDist_("WeaponMirv::hspreadDist", 0), 
	vspreadDist_("WeaponMirv::vspreadDist", 0),
	aimedWeapon_(0)
{

}

WeaponMirv::~WeaponMirv()
{
	delete aimedWeapon_;
	aimedWeapon_ = 0;
}

bool WeaponMirv::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the accessory spread
	if (!accessoryNode->getNamedChild("hspreaddist", hspreadDist_)) 
		return false;
	if (!accessoryNode->getNamedChild("vspreaddist", vspreadDist_)) 
		return false;

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

	// Get the accessory warheads
	if (!accessoryNode->getNamedChild("nowarheads", noWarheads_)) return false;

	return true;
}

void WeaponMirv::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Add a shot that will fall where the original was aimed
	aimedWeapon_->fire(context, weaponContext, position, velocity);

	RandomGenerator &random = context.getSimulator().getRandomGenerator();

	// Add all of the sub warheads that have a random spread
	fixed hspreadDist;
	for (int i=0; i<noWarheads_ - 1; i++)
	{
		FixedVector newDiff = velocity;
		newDiff[2] = 0;
		// Ensure the same value is used in all parts of the calc
		hspreadDist = hspreadDist_.getValue(context);
		if (hspreadDist != 0)
		{
			FixedVector diff = newDiff;
			diff[2] -= 1;
			FixedVector perp = newDiff * diff;

			newDiff += (perp * ((random.getRandFixed("WeaponMirv") * hspreadDist) - (hspreadDist * fixed(true, 5000))));
		}
		newDiff[2] += (fixed(i - (noWarheads_ / 2)) / 
			fixed(noWarheads_ / 2)) * vspreadDist_.getValue(context);

		aimedWeapon_->fire(context, weaponContext, position, newDiff);
	}
}

