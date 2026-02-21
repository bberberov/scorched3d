////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_WeaponAimedOver_hpp_INCLUDE__
#define __INCLUDE_WeaponAimedOver_hpp_INCLUDE__

#include <weapons/WeaponAimed.hpp>

class WeaponAimedOver : public WeaponAimed
{
public:
	WeaponAimedOver();
	virtual ~WeaponAimedOver();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	REGISTER_ACCESSORY_HEADER(WeaponAimedOver, AccessoryPart::AccessoryWeapon);

protected:
	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	virtual void aimShot(ScorchedContext &context,
		RandomGenerator &random,
		FixedVector &position, FixedVector &shootAt,
		fixed &angleXYDegs, fixed &angleYZDegs, fixed &power);
};

#endif // __INCLUDE_WeaponAimedOver_hpp_INCLUDE__
