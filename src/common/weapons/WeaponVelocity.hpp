////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_WeaponVelocity_hpp_INCLUDE__
#define __INCLUDE_WeaponVelocity_hpp_INCLUDE__

#include <weapons/Weapon.hpp>

class WeaponVelocity : public Weapon
{
public:
	WeaponVelocity();
	virtual ~WeaponVelocity();

	virtual bool parseXML( AccessoryCreateContext& context, XMLNode* accessoryNode );

	// Inherited from Weapon
	void fireWeapon(
		ScorchedContext&   context,
		WeaponFireContext& weaponContext,
		FixedVector&       position,
		FixedVector&       velocity
	);

	REGISTER_ACCESSORY_HEADER( WeaponVelocity, AccessoryPart::AccessoryWeapon );

protected:
	NumberParser velocityChange_;
	bool         abs_;
	Weapon*      aimedWeapon_;
};

#endif  // __INCLUDE_WeaponVelocity_hpp_INCLUDE__
