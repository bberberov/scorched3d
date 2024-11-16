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

#if !defined(AFX_WEAPONLEAPFROG_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
#define AFX_WEAPONLEAPFROG_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_

#include <weapons/Weapon.h>

class WeaponLeapFrog : public Weapon
{
public:
	WeaponLeapFrog();
	virtual ~WeaponLeapFrog();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	Weapon *getCollisionAction() { return collisionAction_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLeapFrog, AccessoryPart::AccessoryWeapon);

protected:
	NumberParser bounce_;
	Weapon *collisionAction_;

};

#endif // !defined(AFX_WEAPONLEAPFROG_H__70119A64_2064_4066_8EE5_FD6A3E24D5FC__INCLUDED_)
