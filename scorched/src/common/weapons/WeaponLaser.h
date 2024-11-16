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

#if !defined(__INCLUDE_WeaponLaserh_INCLUDE__)
#define __INCLUDE_WeaponLaserh_INCLUDE__

#include <weapons/Weapon.h>
#include <actions/LaserParams.h>

class WeaponLaser  : public Weapon
{
public:
	WeaponLaser();
	virtual ~WeaponLaser();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLaser, AccessoryPart::AccessoryWeapon);

protected:
	LaserParams laserParams_;

	// Use the following to hold the NumberParser fixed expression
	// convert to values in actions/Laser
	NumberParser minimumHurt_, maximumHurt_;
	NumberParser minimumDistance_, maximumDistance_;
	NumberParser hurtRadius_;
	NumberParser totalTime_;
};

#endif // __INCLUDE_WeaponLaserh_INCLUDE__
