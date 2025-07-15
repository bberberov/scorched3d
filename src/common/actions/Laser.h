////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#if !defined(__INCLUDE_Laserh_INCLUDE__)
#define __INCLUDE_Laserh_INCLUDE__

#include <actions/Action.h>
#include <weapons/WeaponLaser.h>
#include <actions/LaserParams.h>

class Laser : public Action
{
public:
	Laser(
		Weapon *weapon,
		LaserParams *params,
		FixedVector &position,
		FixedVector &direction,
		WeaponFireContext &weaponContext
	);
	virtual ~Laser();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual void draw();
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "Laser"; }

protected:
	Weapon *weapon_;
	WeaponFireContext weaponContext_;
	LaserParams *params_;
	FixedVector position_;
	FixedVector direction_;
	fixed totalTime_;
	fixed drawLength_;
	fixed directionMagnitude_;

	fixed laserTime_;
	fixed hurtRadius_;
	float angXY_;
	float angYZ_;
	fixed length_;
	fixed damage_;
	bool firstTime_;
};

#endif // __INCLUDE_Laserh_INCLUDE__
