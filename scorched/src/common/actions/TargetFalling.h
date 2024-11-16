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

#if !defined(__INCLUDE_TargetFallingh_INCLUDE__)
#define __INCLUDE_TargetFallingh_INCLUDE__

#include <engine/PhysicsParticle.h>
#include <engine/ScorchedCollisionIds.h>
#include <weapons/Weapon.h>

#include <map>
#include <list>

class Parachute;
class TargetFalling : public PhysicsParticle
{
public:
	TargetFalling(Weapon *weapon, unsigned int fallingPlayerId,
			WeaponFireContext &weaponContext, Parachute *parachute);
	virtual ~TargetFalling();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "TargetFalling"; }

	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);

	Parachute *getParachute() { return parachute_; }

protected:
	Weapon *weapon_;
	Parachute *parachute_;
	unsigned int fallingPlayerId_;
	WeaponFireContext weaponContext_;
	unsigned int data_;
	FixedVector tankStartPosition_;

};

#endif
