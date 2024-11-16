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

#if !defined(__INCLUDE_Explosionh_INCLUDE__)
#define __INCLUDE_Explosionh_INCLUDE__

#include <actions/Action.h>
#include <weapons/Weapon.h>
#include <common/FixedVector.h>

class ExplosionParams;
class Explosion : public Action
{
public:
	Explosion(FixedVector &position, 
		FixedVector &velocity,
		ExplosionParams *params,
		Weapon *weapon, 
		WeaponFireContext &weaponContext);
	virtual ~Explosion();

	FixedVector &getPosition() { return position_; }

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "Explosion"; }
protected:
	ExplosionParams *params_;
	bool firstTime_;
	FixedVector position_, velocity_;
	fixed totalTime_;
	Weapon *weapon_;
	WeaponFireContext weaponContext_;
	
};

#endif
