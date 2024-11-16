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

#include <tankai/TankAIAimGuesser.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <tank/TankLib.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/RandomGenerator.h>
#include <math.h>

TankAIAimGuesser::TankAIAimGuesser(ScorchedContext &context) :
	context_(context)
{
}

TankAIAimGuesser::~TankAIAimGuesser()
{
}

bool TankAIAimGuesser::guess(Tanket *tanket, Vector &target, 
	float angleYZDegs, float distance, 
	Vector &actualPosition)
{
	tanket->getShotInfo().rotateGunYZ(fixed::fromFloat(angleYZDegs), false);

	// Make an initial randomish shot up
	initialShot(tanket, target);

	for (int i=0;;i++)
	{
		// Find out where this may land
		getCurrentGuess(tanket);
		if (!collision_)
		{
			// No collision
			// A bad thing
			return false; 
		}

		// Its landed
		actualPosition = currentGuess_.getPosition().asVector();
		Vector direction = currentGuess_.getPosition().asVector() - target;
		float actualDistance = 
			float(sqrt(direction[0]*direction[0] + direction[1]*direction[1]));
		if (actualDistance < distance)
		{
			// Its close
			// A good thing
			return true;
		}

		if (i > 25)
		{
			// No soultion
			// A bad thing
			return false;
		}

		// Not close
		refineShot(tanket, currentGuess_.getPosition().asVector(), target);
	}

	// Never gets here
	return false;
}

void TankAIAimGuesser::initialShot(Tanket *tanket, Vector &target)
{
	fixed angleXYDegs;
	fixed angleYZDegs;
	fixed power;

	// Makes a randow powered shot towards the target
	FileRandomGenerator random;
	random.seed(rand());
	TankLib::getShotTowardsPosition(
		context_,
		random,
		tanket->getLife().getTargetPosition(), 
		FixedVector::fromVector(target), 
		angleXYDegs, angleYZDegs, power);

	// Set the parameters
	// Sets the angle of the gun and the power
	tanket->getShotInfo().rotateGunXY(angleXYDegs, false);
	tanket->getShotInfo().changePower(power, false);
}

void TankAIAimGuesser::refineShot(Tanket *tanket,
	Vector &currentPos, Vector &wantedPos)
{
	// Get the used velocity
	FixedVector shotVelocity = 
		TankLib::getVelocityVector(tanket->getShotInfo().getRotationGunXY(), tanket->getShotInfo().getRotationGunYZ()) *
		(tanket->getShotInfo().getPower() + 1);

	// Figure out how much the last shot missed by
	Vector missedBy = wantedPos - currentPos;
	
	// Adjust velocity to take this into account
	shotVelocity[0] += fixed::fromFloat(missedBy[0]) * fixed(true, rand() % 200 + 200);
	shotVelocity[1] += fixed::fromFloat(missedBy[1]) * fixed(true, rand() % 200 + 200);

	// Figure out a new XY angle based on these
	fixed angleXYRads = atan2x(shotVelocity[1], shotVelocity[0]);
	fixed angleXYDegs = (angleXYRads / fixed::XPI) * 180 - 90;

	tanket->getShotInfo().rotateGunXY(angleXYDegs, false);

	// And the new best power
	float dist = (currentPos - tanket->getLife().getTargetPosition().asVector()).Magnitude2d();
	float wanteddist = (wantedPos - tanket->getLife().getTargetPosition().asVector()).Magnitude2d();
	float currentPower = (float) (log(dist) / log(2.0));
	float wantedPower = (float) (log(wanteddist) / log(2.0));

	fixed power = tanket->getShotInfo().getPower() * fixed::fromFloat(wantedPower) / 
		fixed::fromFloat(currentPower);

	tanket->getShotInfo().changePower(power, false);
}

void TankAIAimGuesser::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	currentGuess_ = position;
	collision_ = true;
}

void TankAIAimGuesser::wallCollision(PhysicsParticleObject &position,
	ScorchedCollisionId collisionId)
{
	// For now the AIs cannot cope with bouncy and wrap walls.
	// So they will count the shot end where they hit the wall
	currentGuess_ = position;
	collision_ = true;
}

void TankAIAimGuesser::getCurrentGuess(Tanket *tanket)
{
	bool actionSyncCheck = context_.getOptionsGame().
		getMainOptions().getActionSyncCheck();
	context_.getOptionsGame().getMainOptions().
		getActionSyncCheckEntry().setValue(false);

	FixedVector shotVelocity = TankLib::getVelocityVector(tanket->getShotInfo().getRotationGunXY(), tanket->getShotInfo().getRotationGunYZ()) *
		(tanket->getShotInfo().getPower() + 1);
	FixedVector shotPosition = TankLib::getTankGunPosition(tanket->getLife().getTankTurretPosition(),
		tanket->getShotInfo().getRotationGunXY(), tanket->getShotInfo().getRotationGunYZ());

	PhysicsParticleObject particleObject;
	PhysicsParticleInfo info(ParticleTypeShot, tanket->getPlayerId(), 0);
	particleObject.setPhysics(info, context_,
		shotPosition, shotVelocity);
	particleObject.setHandler(this);

	collision_ = false;
	for (int i=0; i<10000 && !collision_; i++)
	{
		particleObject.simulate(1);
	}

	context_.getOptionsGame().getMainOptions().
		getActionSyncCheckEntry().setValue(actionSyncCheck);
}
