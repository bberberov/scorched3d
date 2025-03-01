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

#if !defined(AFX_WeaponAimedOver_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_)
#define AFX_WeaponAimedOver_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_

#include <weapons/WeaponAimed.h>

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

#endif // !defined(AFX_WeaponAimedOver_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_)
