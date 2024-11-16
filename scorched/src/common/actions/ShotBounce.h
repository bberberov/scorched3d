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

#if !defined(__INCLUDE_ShotBounceh_INCLUDE__)
#define __INCLUDE_ShotBounceh_INCLUDE__

#include <engine/PhysicsParticle.h>
#include <engine/ScorchedCollisionIds.h>
#include <actions/ParticleGroup.h>
#include <weapons/WeaponRoller.h>

class TankViewPointProvider;
class ModelRendererSimulator;
class ShotBounce : public PhysicsParticle
{
public:
	ShotBounce(
		WeaponRoller *weapon, 
		FixedVector &startPosition, FixedVector &velocity,
		WeaponFireContext &weaponContext);
	virtual ~ShotBounce();

	virtual void simulate(fixed frameTime, bool &remove);
	virtual void init();
	virtual void draw();
	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "ShotBounce"; }

	unsigned int getPlayerId() { return weaponContext_.getPlayerId(); }
	WeaponRoller *getWeapon() { return weapon_; }

protected:
	ParticleGroup *groups_;
	TankViewPointProvider *vPoint_;
	FixedVector startPosition_, velocity_;
	FixedVector lookFrom_;
	WeaponRoller *weapon_;
	WeaponFireContext weaponContext_;
	fixed totalTime_, simulateTime_, stepSize_, timeout_;
	fixed weaponTime_;
	ModelRendererSimulator *model_;
	float scale_;
	bool collided_;

	void doCollision();

private:
	ShotBounce(const ShotBounce &);
	const ShotBounce & operator=(const ShotBounce &);

};

#endif
