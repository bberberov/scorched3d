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

#if !defined(AFX_TargetDamageCalc_H__3B055B6D_C8E7_42C7_9D29_FBBFD88457D1__INCLUDED_)
#define AFX_TargetDamageCalc_H__3B055B6D_C8E7_42C7_9D29_FBBFD88457D1__INCLUDED_

#include <engine/ScorchedContext.h>
#include <common/FixedVector.h>

class Target;
class Weapon;
class WeaponFireContext;
class TargetDamageCalc
{
public:
	static void explosion(ScorchedContext &context,
		Weapon *weapon, WeaponFireContext &weaponContext,
		FixedVector &position, fixed radius,
		fixed damageAmount,
		bool checkFall,
		bool shieldOnlyDamage);
	static void damageTarget(ScorchedContext &context,
		unsigned int playerId, 
		Weapon *weapon, WeaponFireContext &weaponContext,
		fixed damage,
		bool useShieldDamage, bool checkFall,
		bool shieldOnlyDamage);

private:
	TargetDamageCalc();
	virtual ~TargetDamageCalc();

};

#endif // !defined(AFX_TargetDamageCalc_H__3B055B6D_C8E7_42C7_9D29_FBBFD88457D1__INCLUDED_)
