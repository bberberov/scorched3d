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

#include <actions/ShotProjectile.h>
#include <actions/CameraPositionAction.h>
#ifndef S3D_SERVER
	#include <sprites/MissileActionRenderer.h>
	#include <tankgraph/RenderTracer.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <tank/TankViewPoints.h>
#include <target/TargetContainer.h>
#include <tankai/TankAI.h>
#include <tanket/Tanket.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <math.h>

ShotProjectile::ShotProjectile(FixedVector &startPosition, FixedVector &velocity,
							   WeaponProjectile *weapon, WeaponFireContext &weaponContext,
							   unsigned int flareType,
							   fixed spinSpeed, const Vector &spinAxis) :
	PhysicsParticle(weaponContext.getInternalContext().getReferenced()),
	startPosition_(startPosition), velocity_(velocity), 
	weapon_(weapon), weaponContext_(weaponContext), 
	flareType_(flareType), vPoint_(0),
	snapTime_(fixed(true, 2000)), up_(false), collided_(false),
	totalTime_(0), simulateTime_(0), 
	spinSpeed_(spinSpeed), spinAxis_(spinAxis),
	groups_(0), physicsSpin_(0)
{
}

void ShotProjectile::init()
{
	fixed weaponScale = weapon_->getScale(*context_);
#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		setActionRender(new MissileActionRenderer(flareType_, 
				weaponScale.asFloat(),
				spinSpeed_.asFloat(),
				spinAxis_));

		if (!weapon_->getNoCameraTrack())
		{
			vPoint_ = new TankViewPointProvider();
			vPoint_->incrementReference();
			FixedVector velocity = velocity_;
			velocity[2] = 10;
			vPoint_->setValues(startPosition_, velocity);
		
			CameraPositionAction *positionAction = new CameraPositionAction(
				weaponContext_.getPlayerId(),
				vPoint_,
				5,
				10,
				false);
			context_->getActionController().addAction(positionAction);
		}
	}
#endif // #ifndef S3D_SERVER

	PhysicsParticleInfo info(ParticleTypeShot, weaponContext_.getPlayerId(), this);
	getPhysics().setPhysics(info, *context_, startPosition_, velocity_);
	getPhysics().setForces(weapon_->getWindFactor(*context_), weapon_->getGravityFactor(*context_));
	getPhysics().setOptionUnderGroundCollision(getWeapon()->getUnder());
	getPhysics().setOptionWallCollision(getWeapon()->getWallCollision());
	getPhysics().setOptionLandscapeCollision(getWeapon()->getLandscapeCollision());
	getPhysics().setOptionShieldCollision(getWeapon()->getShieldCollision());
	getPhysics().setOptionTankCollision(getWeapon()->getTankCollision());
	getPhysics().setOptionTargetCollision(getWeapon()->getTargetCollision());

	thrustTime_ = getWeapon()->getThrustTime(*context_);
	timeout_ = weapon_->getTimeout(*context_);
	thrustAmount_ = getWeapon()->getThrustAmount(*context_);
	timedCollision_ = getWeapon()->getTimedCollision(*context_);
	heightCollision_ = getWeapon()->getHeightCollision(*context_);
	wobbleSpin_ = getWeapon()->getWobbleSpin(*context_);
	wobbleAmount_= getWeapon()->getWobbleAmount(*context_);
	drag_ = getWeapon()->getDrag(*context_);
	stepSize_ = getWeapon()->getStepSize() * 
		fixed(true, context_->getOptionsGame().getWeaponSpeed());

	if (weapon_->getLocalGroups().hasGroups() || weapon_->getGlobalGroups().hasGroups())
	{
		groups_ = new ParticleGroup(*context_, this, &weaponContext_);
		weapon_->getLocalGroups().addToGroups(weaponContext_.getInternalContext().getLocalGroups(), groups_);
		weapon_->getGlobalGroups().addToGroups(context_->getObjectGroups(), groups_);
	}
}

std::string ShotProjectile::getActionDetails()
{
	return S3D::formatStringBuffer("%s %s %s",
		startPosition_.asQuickString(),
		velocity_.asQuickString(),
		weapon_->getParent()->getName());
}

ShotProjectile::~ShotProjectile()
{
	if (vPoint_) vPoint_->decrementReference();
	delete groups_;
}

void ShotProjectile::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	if (!collision_)
	{
		// Tell all AIs about this collision
		std::map<unsigned int, Tanket *> &tanks = 
			context_->getTargetContainer().getTankets();
		std::map<unsigned int, Tanket *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tanket *tanket = (*itor).second;
			TankAI *ai = tanket->getTankAI();
			if (ai)
			{		
				if (tanket->getAlive())
				{
					ai->shotLanded(collisionId, 
						getWeapon(), getPlayerId(), 
						getPhysics().getPosition().asVector());
				}
			}
		}

		bool doColl = true;

		// Apex collisions dud if they collide with the ground
		// unless no dud is set
		if (getWeapon()->getApexCollision() && !getWeapon()->getApexNoDud())
		{
			doColl = false;
		}
		if ((timedCollision_ > 0) && getWeapon()->getTimedDud())
		{
			doColl = false;
		}

		if (doColl) doCollision(position.getPosition());
	}
	PhysicsParticle::collision(position, collisionId);
}

void ShotProjectile::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;

	// Water collision
	if (!remove &&
		getWeapon()->getWaterCollision())
	{
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;

			waterHeight = water->height;
		}

		if (getPhysics().getPosition()[2] < waterHeight)
		{
			doCollision(getPhysics().getPosition());
			remove = true;
		}
	}

	// Apex collision
	if (!remove &&
		getWeapon()->getApexCollision())
	{
		if (getPhysics().getVelocity()[2] > 0) up_ = true;
		else if (up_)
		{
			doCollision(getPhysics().getPosition());
			remove = true;
		}
	}

	// Timeout
	if (timeout_ > 0 && 
		totalTime_ > timeout_)
	{
		remove = true;
	}

	// Height collision
	if (!remove &&
		heightCollision_ > 0)
	{
		if (getPhysics().getVelocity()[2] < 0)
		{
			fixed landHeight = context_->getLandscapeMaps().
				getGroundMaps().getInterpHeight(
					getPhysics().getPosition()[0], 
					getPhysics().getPosition()[1]);
			if (getPhysics().getPosition()[2] <= heightCollision_ + landHeight)
			{
				doCollision(getPhysics().getPosition());
				remove = true;
			}
		}
	}

 	if (wobbleSpin_ > 0)
	{
		FixedVector up(0, 0, 1);
		FixedVector velocityPerp = (velocity_.Normalize() * up).Normalize();
		if (velocityPerp == FixedVector::getNullVector()) 
		{
			velocityPerp = FixedVector(1, 0, 0);
		}
		FixedVector forceDir = ((velocityPerp * physicsSpin_.cos()) * wobbleAmount_ * velocity_.Magnitude() / 50);
		getPhysics().applyOffset(forceDir);

		physicsSpin_ += wobbleSpin_;
	}

	// Thrust
	if (thrustAmount_ > 0)
	{
		if (totalTime_ < thrustTime_ ||
			thrustTime_ == 0)
		{
			FixedVector direction = getPhysics().getVelocity();
			direction.StoreNormalize();
			direction *= thrustAmount_;
			getPhysics().applyForce(direction);
		}
	}

	// Drag
	if (drag_ > 0)
	{
		FixedVector direction = getPhysics().getVelocity();
		direction *= -drag_;
		getPhysics().applyForce(direction);
	}

	// Timed collision
	if (!remove &&
		timedCollision_ > 0)
	{
		if (totalTime_ > timedCollision_)
		{
			doCollision(getPhysics().getPosition());
			remove = true;
		}
	}

	// Shot path
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (getWeapon()->getShowShotPath())
		{
			snapTime_ += frameTime;
			if (snapTime_.asFloat() > 0.1f || remove)
			{
				Vector up (0.0f, 0.0f, 1.0f);
				RenderTracer::TracerLinePoint point;
				point.position = getPhysics().getPosition().asVector();
				point.cross = (getPhysics().getVelocity().asVector() * up).Normalize();
				positions_.push_back(point);
				snapTime_ = 0;
			}
		}
	}
#endif	// #ifndef S3D_SERVER

	simulateTime_ += frameTime;
	while (simulateTime_ > stepSize_)
	{
		PhysicsParticle::simulate(stepSize_, remove);
		simulateTime_ -= stepSize_;
	}

	if (vPoint_)
	{
		FixedVector velocity = -getPhysics().getVelocity();
		velocity[2] = 10;
		vPoint_->setValues(getPhysics().getPosition(), velocity);
	}
}

void ShotProjectile::doCollision(FixedVector &position)
{	
	if (collided_) return;
	collided_ = true;

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (getWeapon()->getShowShotPath())
		{
			RenderTracer::instance()->
				addSmokeTracer(weaponContext_.getPlayerId(), 
					position.asVector(), positions_);
		}
		else if (getWeapon()->getShowEndPoint())
		{
			RenderTracer::instance()->
				addTracer(weaponContext_.getPlayerId(), position.asVector());
		}
	}
#endif // #ifndef S3D_SERVER

	FixedVector velocity;
	getWeapon()->getCollisionAction()->fire(
		*context_, weaponContext_, position, 
		getPhysics().getVelocity());
}

