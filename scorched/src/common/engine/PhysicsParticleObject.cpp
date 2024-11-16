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

#include <engine/PhysicsParticleObject.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/Tank.h>
#include <target/TargetSpace.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <target/TargetContainer.h>
#include <weapons/Accessory.h>
#include <weapons/Shield.h>
#include <weapons/ShieldRoundReflective.h>
#include <weapons/ShieldSquareReflective.h>
#include <weapons/ShieldRoundMag.h>
#include <actions/ShotBounce.h>
#include <actions/ShotProjectile.h>
#include <actions/ShieldHit.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <common/Logger.h>

PhysicsParticleObject::PhysicsParticleObject() : 
	handler_(0), context_(0), optionUnderGroundCollision_(false), iterations_(0),
	info_(ParticleTypeNone, 0, 0), optionRotateOnCollision_(false), optionWallCollision_(true),
	optionStickyShields_(false), optionShieldCollision_(true), optionLandscapeCollision_(true),
	optionTankCollision_(true), optionTargetCollision_(true)
{
}

PhysicsParticleObject::~PhysicsParticleObject()
{
}

void PhysicsParticleObject::applyForce(FixedVector &force)
{
	velocity_ += force;
}

void PhysicsParticleObject::applyOffset(FixedVector &offset)
{
	position_ += offset;
}

void PhysicsParticleObject::setPhysics(
	PhysicsParticleInfo info,
	ScorchedContext &context,
	FixedVector &position, FixedVector &velocity)
{
	info_ = info;
	context_ = &context;

	FixedVector zaxis(0, 0, 1);
	rotation_.setQuatFromAxisAndAngle(zaxis, 0);
	position_ = position;
	velocity_ = velocity;

	setForces(1, 1);
}

void PhysicsParticleObject::setForces(
	fixed windFactor, fixed gravityFactor) 
{
	windFactor_ = 
		context_->getSimulator().getWind().getWindDirection() * 
		context_->getSimulator().getWind().getWindSpeed() / 
		fixed(true, 25000) * windFactor;

	FixedVector gravity(0, 0, context_->getOptionsGame().getGravity());
	windFactor_ += gravity * gravityFactor;
	windFactor_ /= 70;
}

void PhysicsParticleObject::simulate(fixed frameTime)
{
	iterations_++;
	velocity_ += windFactor_;
	position_ += velocity_ / 100;

	if (optionRotateOnCollision_)
	{
		rotation_ += avelocity_;
		rotation_.Normalize();
	}

	if (!handler_ || !context_) return;

	checkCollision();
}

void PhysicsParticleObject::checkCollision()
{
	CollisionInfo collision;
	collision.collisionId = CollisionIdNone;

	fixed bounceFactor = 2;

	// Find if we have had a collision
	CollisionAction action = CollisionActionNone;
	Target *target = context_->getTargetSpace().getCollision(position_);
	switch (info_.type_)
	{
	case ParticleTypeShot:
		if (getTargetCollision(collision, target) ||
			getShieldCollision(collision, target) ||
			getLandscapeCollision(collision) ||
			getRoofCollision(collision) ||
			getWallCollision(collision)) 
		{
			action = checkShotCollision(collision, target);

			if (action == CollisionActionBounce &&
				collision.collisionId == CollisionIdShield &&
				optionStickyShields_)
			{
				bounceFactor = fixed(true, 1750);
			}

			if (action != CollisionActionNone)
			{
				if (context_->getOptionsGame().getActionSyncCheck())
				{
					context_->getSimulator().addSyncCheck(
						S3D::formatStringBuffer("Shot Collision : %i %i %s", 
							(int) action,
							(int) collision.collisionId,
							position_.asQuickString()));
				}
			}
		}
		break;
	case ParticleTypeBounce:
		if (getTargetBounceCollision(collision, target) ||
			getTargetCollision(collision, target) ||
			getShieldCollision(collision, target) ||
			getLandscapeCollision(collision) ||
			getWallCollision(collision))
		{
			action = checkBounceCollision(collision, target);

			velocity_[2] = MIN(velocity_[2], 1);

			if (collision.collisionId != CollisionIdShield ||
				optionStickyShields_)
			{
				bounceFactor = fixed(true, 1750);
			}
		}
		break;
	case ParticleTypeFalling:
		if (getLandscapeCollision(collision) ||
			getWallCollision(collision))
		{
			action = checkFallingCollision(collision, target);
		}
		break;
	}	

	// Perform the result of the collision (if any)
	switch (action)
	{
	case CollisionActionCollision:
		handler_->collision(*this, collision.collisionId);
		break;
	case CollisionActionBounce:
		{
			// Move the shot back to the original position so that the
			// shot is outside the object after the bounce
			position_ -= velocity_ / 100;

			// Calculate the new bounce position
			fixed strength = velocity_.Magnitude();
			FixedVector direction = velocity_ / strength;
			fixed dotp = -collision.normal.dotP(direction);
			direction = direction + collision.normal * (dotp * bounceFactor);
			velocity_ = direction * strength * collision.deflectFactor;

			fixed landHeight = 
				context_->getLandscapeMaps().getGroundMaps().
					getInterpHeight(position_[0], position_[1]);
			fixed particleHeight = position_[2] - landHeight;
			if (optionUnderGroundCollision_)
			{
				if (particleHeight > fixed(true, -1000)) 
					position_[2] = landHeight - fixed(true, 1000);
			}
			else
			{
				if (particleHeight < fixed(true, 1000)) 
					position_[2] = landHeight + fixed(true, 1000);
			}

			if (optionRotateOnCollision_)
			{
				FixedVector up(0, 0, -1);
				FixedVector rotAxis = velocity_ * up;
				rotAxis.StoreNormalize();
				avelocity_.setQuatFromAxisAndAngle(rotAxis, velocity_.Magnitude() * 5);
			}
		}
		break;
	}
}

PhysicsParticleObject::CollisionAction PhysicsParticleObject::checkShotCollision(
	CollisionInfo &collision, Target *target)
{
	int arenaX = context_->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context_->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context_->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context_->getLandscapeMaps().getGroundMaps().getArenaHeight();

	switch(collision.collisionId)
	{
	case CollisionIdLandscape:
	case CollisionIdRoof:
	case CollisionIdTarget:
		return CollisionActionCollision;
		break;
	case CollisionIdShield:
		if (target && target->getShield().getCurrentShield())
		{
			shotShieldHit(target);

			if (target->getShield().getCurrentShield()) // shotShieldHit may have removed shield
			{
				Shield *shield = (Shield *) target->getShield().getCurrentShield()->getAction();
				switch (shield->getShieldType())
				{
				case Shield::ShieldTypeRoundNormal:
				case Shield::ShieldTypeSquareNormal:
					return CollisionActionCollision;
				case Shield::ShieldTypeRoundReflective:
				case Shield::ShieldTypeSquareReflective:
					return CollisionActionBounce;
				case Shield::ShieldTypeRoundMag:
					{
						ShieldRoundMag *magShield = (ShieldRoundMag *) shield;
						FixedVector force(0, 0, magShield->getDeflectPower() / 50);
						velocity_ += force;
					}
					return CollisionActionNone;
				}
			}
		}
		break;
	case CollisionIdWallLeft:
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:

		switch(collision.collisionId)
		{
		case CollisionIdWallLeft:
			position_[0] = arenaX + 1;
			break;
		case CollisionIdWallRight:
			position_[0] = arenaX + arenaWidth - 1;
			break;
		case CollisionIdWallTop:
			position_[1] = arenaY + 1;
			break;
		case CollisionIdWallBottom:
			position_[1] = arenaY + arenaHeight - 1;
			break;
		}

		shotWallHit(collision);

		handler_->wallCollision(*this, collision.collisionId);

		switch (context_->getOptionsTransient().getWallType())
		{
		case OptionsTransient::wallBouncy:
			return CollisionActionBounce;
		case OptionsTransient::wallWrapAround:

			switch (collision.collisionId)
			{
			case CollisionIdWallLeft:
				position_[0] = arenaX + arenaWidth - 10;
				break;
			case CollisionIdWallRight:
				position_[0] = arenaX + 10;
				break;
			case CollisionIdWallTop:
				position_[1] = arenaY + arenaHeight - 10;
				break;
			case CollisionIdWallBottom:
				position_[1] = arenaX + 10;
				break;
			}
			return CollisionActionNone;
		case OptionsTransient::wallConcrete:
			return CollisionActionCollision;
		}
		break;
	}
	return CollisionActionNone;
}

PhysicsParticleObject::CollisionAction PhysicsParticleObject::checkBounceCollision(
	CollisionInfo &collision, Target *target)
{
	int arenaX = context_->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context_->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context_->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context_->getLandscapeMaps().getGroundMaps().getArenaHeight();

	switch(collision.collisionId)
	{
	case CollisionIdRoof:
		return CollisionActionNone;
		break;
	case CollisionIdLandscape:
		return CollisionActionBounce;
	case CollisionIdWallLeft:
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:

		if (context_->getOptionsTransient().getWallType() == OptionsTransient::wallBouncy ||
			context_->getOptionsTransient().getWallType() == OptionsTransient::wallConcrete)
		{
			switch(collision.collisionId)
			{
			case CollisionIdWallLeft:
				position_[0] = arenaX + 1;
				break;
			case CollisionIdWallRight:
				position_[0] = arenaX + arenaWidth - 1;
				break;
			case CollisionIdWallTop:
				position_[1] = arenaY + 1;
				break;
			case CollisionIdWallBottom:
				position_[1] = arenaY + arenaHeight - 1;
				break;
			}
		}

		switch (context_->getOptionsTransient().getWallType())
		{
		case OptionsTransient::wallBouncy:
			return CollisionActionBounce;
		case OptionsTransient::wallWrapAround:
			return CollisionActionNone;
		case OptionsTransient::wallConcrete:
			return CollisionActionCollision;
		}
		break;
	case CollisionIdShield:
		bounceShieldHit(target);
		return CollisionActionBounce;
		break;
	case CollisionIdTarget:
		return CollisionActionCollision;
		break;
	}
	return CollisionActionNone;
}

PhysicsParticleObject::CollisionAction PhysicsParticleObject::checkFallingCollision(
	CollisionInfo &collision, Target *target)
{
	int arenaX = context_->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context_->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context_->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context_->getLandscapeMaps().getGroundMaps().getArenaHeight();

	switch(collision.collisionId)
	{
	case CollisionIdTarget:
	case CollisionIdShield:
		return CollisionActionNone;
		break;
	case CollisionIdRoof:
	case CollisionIdWallLeft:
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:
		switch(collision.collisionId)
		{
		case CollisionIdWallLeft:
			position_[0] = arenaX + 1;
			break;
		case CollisionIdWallRight:
			position_[0] = arenaX + arenaWidth - 1;
			break;
		case CollisionIdWallTop:
			position_[1] = arenaY + 1;
			break;
		case CollisionIdWallBottom:
			position_[1] = arenaY + arenaHeight - 1;
			break;
		}

		return CollisionActionBounce;
		break;
	case CollisionIdLandscape:
		return CollisionActionCollision;
		break;
	}
	return CollisionActionNone;
}

bool PhysicsParticleObject::getLandscapeCollision(CollisionInfo &collision)
{
	if (optionUnderGroundCollision_)
	{
		if (position_[2] <= context_->getOptionsGame().getMinimumLandHeight()) 
		{
			collision.collisionId = CollisionIdLandscape;
			collision.deflectFactor = 1;
			collision.normal = FixedVector(0, 0, -1);
			return true;
		}
		// Check for collision with the ground
		// (or underground collision if applicable)
		fixed landHeight = 
			context_->getLandscapeMaps().getGroundMaps().
				getInterpHeight(position_[0], position_[1]);
		if (optionLandscapeCollision_ &&
			position_[2] >= landHeight)
		{
			collision.collisionId = CollisionIdLandscape;
			collision.deflectFactor = 1;
			context_->getLandscapeMaps().getGroundMaps().
				getInterpNormal(position_[0], position_[1], collision.normal);
			collision.normal = -collision.normal;
			return true;
		}
	}
	else
	{
		if (position_[2] <= context_->getOptionsGame().getMinimumLandHeight())
		{
			collision.collisionId = CollisionIdLandscape;
			collision.deflectFactor = 1;
			collision.normal = FixedVector(0, 0, 1);
			return true;
		}
		// Check for collision with the ground
		// (or underground collision if applicable)
		fixed landHeight = 
			context_->getLandscapeMaps().getGroundMaps().
				getInterpHeight(position_[0], position_[1]);
		if (optionLandscapeCollision_ &&
			position_[2] <= landHeight)
		{
			collision.collisionId = CollisionIdLandscape;
			collision.deflectFactor = 1;
			context_->getLandscapeMaps().getGroundMaps().
				getInterpNormal(position_[0], position_[1], collision.normal);
			return true;
		}
	}
	return false;
}

bool PhysicsParticleObject::getRoofCollision(CollisionInfo &collision)
{
	// This will return MAX_FLT when there is no roof
	fixed maxHeight = context_->getLandscapeMaps().getRoofMaps().getInterpRoofHeight(
		position_[0], position_[1]);
	if (position_[2] >= maxHeight)
	{
		collision.collisionId = CollisionIdRoof;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(0, 0, -1);
		return true;
	}
	return false;
}

bool PhysicsParticleObject::getWallCollision(CollisionInfo &collision)
{
	// Check for collision with the walls (if enabled)
	if (context_->getOptionsTransient().getWallType() == OptionsTransient::wallNone)
	{
		return false;
	}

	// Check if we collide with walls
	if (!optionWallCollision_) return false;

	int arenaX = context_->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = context_->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = context_->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = context_->getLandscapeMaps().getGroundMaps().getArenaHeight();
	if (position_[0] <= arenaX)
	{
		collision.collisionId = CollisionIdWallLeft;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(1, 0, 0);
		return true;
	}
	else if (position_[0] >= arenaX + arenaWidth)
	{
		collision.collisionId = CollisionIdWallRight;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(-1, 0, 0);
		return true;
	}
	else if (position_[1] <= arenaY)
	{
		collision.collisionId = CollisionIdWallTop;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(0, 1, 0);
		return true;
	}
	else if (position_[1] >= arenaY + arenaHeight)
	{
		collision.collisionId = CollisionIdWallBottom;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(0, -1, 0);
		return true;
	}
	return false;
}

bool PhysicsParticleObject::getShieldCollision(CollisionInfo &collision, Target *target)
{
	if (!optionShieldCollision_) return false;
	if (!target) return false;

	// Get the shield
	Accessory *shieldAcc = target->getShield().getCurrentShield();			
	if (!shieldAcc) return false;
	Shield *shield = (Shield *) shieldAcc->getAction();

	// Check if this is the shield of the current tank/target
	if (target->getPlayerId() == info_.playerId_)
	{
		return false;
	}

	// Is this tank in the shield
	// This should always be a tank as it is the one firing
	Tank *shotTank = context_->getTargetContainer().getTankById(info_.playerId_);
	if (shotTank)
	{
		FixedVector offset = shotTank->getLife().getTargetPosition() -
			target->getLife().getTargetPosition();
		if (shield->tankInShield(offset))
		{
			// We can ignore this shield as this tank is in the shield
			return false;
		}
	}

	// Check we are in this shield
	FixedVector direction = position_ - target->getLife().getTargetPosition();
	if (!shield->inShield(direction)) return false;

	// Perform the shield action
	switch (shield->getShieldType())
	{
	case Shield::ShieldTypeRoundMag:
		collision.collisionId = CollisionIdShield;
		collision.deflectFactor = 1;
		collision.normal = FixedVector(0, 0, 1);
		return true;
	case Shield::ShieldTypeRoundNormal:
	case Shield::ShieldTypeRoundReflective:
		{
			// Find the deflect factor
			fixed deflectFactor = 1;
			if (shield->getShieldType() == Shield::ShieldTypeRoundReflective)
			{
				ShieldRoundReflective *squareRoundReflective = 
					(ShieldRoundReflective *) shield;
				deflectFactor = squareRoundReflective->getDeflectFactor();
			}

			collision.collisionId = CollisionIdShield;
			collision.deflectFactor = deflectFactor;
			collision.normal = (position_ - target->getLife().getTargetPosition()).Normalize();
		}
		return true;
	case Shield::ShieldTypeSquareReflective:
	case Shield::ShieldTypeSquareNormal:
		{
			// Find the deflect factor
			fixed deflectFactor = 1;
			if (shield->getShieldType() == Shield::ShieldTypeSquareReflective)
			{
				ShieldSquareReflective *squareSquareReflective = 
					(ShieldSquareReflective *) shield;
				deflectFactor = squareSquareReflective->getDeflectFactor();
			}

			// Find the smallest penetration side
			ShieldSquare *squareShield = (ShieldSquare *) shield;
			FixedVector &size = squareShield->getSize();

			fixed diff0 = size[0] - (direction[0].abs());
			fixed diff1 = size[1] - (direction[1].abs());
			fixed diff2 = size[2] - (direction[2].abs());
			if (diff0 <= diff1 && diff0 <= diff2)
			{
				if (direction[0] > 0) collision.normal = FixedVector(-1, 0, 0);
				else collision.normal = FixedVector(1, 0, 0);
			}
			if (diff1 <= diff0 && diff1 <= diff2)
			{
				if (direction[1] > 0) collision.normal = FixedVector(0, -1, 0);
				else collision.normal = FixedVector(0, 1, 0);
			}
			if (diff2 <= diff0 && diff2 <= diff1)
			{
				if (direction[2] > 0) collision.normal = FixedVector(0, 0, -1);
				else collision.normal = FixedVector(0, 0, 1);
			}

			collision.collisionId = CollisionIdShield;
			collision.deflectFactor = deflectFactor;
		}
		return true;
	}

	return false;
}

bool PhysicsParticleObject::getTargetCollision(CollisionInfo &collision, Target *target)
{
	if (!target || !optionTargetCollision_) return false;
	if (target->getType() == Target::TypeTank && !optionTankCollision_) return false;

	// We cannot collide with ourselves
	if (target->getPlayerId() == info_.playerId_) return false;

	// Check we are in this target
	if (target->getLife().collision(position_))
	{
		collision.normal = (position_ - target->getLife().getCenterPosition()).Normalize();	
		collision.deflectFactor = 1;
		collision.collisionId = CollisionIdTarget;
		return true;
	}
	
	return false;
}

bool PhysicsParticleObject::getTargetBounceCollision(CollisionInfo &collision, Target *notUsedTarget)
{
	if (info_.type_ != ParticleTypeBounce) return false;
	if (!optionTargetCollision_) return false;

	// A special case, to add some width to the bounce particle to make it easier
	// to hit targets with
	std::map<unsigned int, Target *> collisionTargets;
	context_->getTargetSpace().getCollisionSet(position_, 1, collisionTargets, false);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		++itor)
	{
		Target *target = (*itor).second;
		if (target->getType() == Target::TypeTank && !optionTankCollision_) continue;
		if (target->getLife().collision(position_) ||
			target->getLife().collisionDistance(position_) < 1)
		{
			collision.normal = (position_ - target->getLife().getCenterPosition()).Normalize();	
			collision.deflectFactor = 1;
			collision.collisionId = CollisionIdTarget;
			return true;
		}
	}

	return false;
}

void PhysicsParticleObject::shotShieldHit(Target *target)
{
}

void PhysicsParticleObject::bounceShieldHit(Target *target)
{
}

void PhysicsParticleObject::shotWallHit(CollisionInfo &collision)
{
}

PhysicsParticleActionObject::PhysicsParticleActionObject()
{
}

PhysicsParticleActionObject::~PhysicsParticleActionObject()
{
}

void PhysicsParticleActionObject::shotShieldHit(Target *target)
{
	Shield *shield = (Shield *) target->getShield().getCurrentShield()->getAction();
	ShotProjectile *shot = (ShotProjectile *) info_.data_;
	fixed hurtFactor = shot->getWeapon()->getShieldHurtFactor(*context_);
	if (hurtFactor > 0)
	{
		if (shield->getShieldType() != Shield::ShieldTypeRoundMag)
		{
			context_->getActionController().addAction(
				new ShieldHit(target->getPlayerId(),
					position_,
					hurtFactor));
		}
		else
		{
			target->getShield().setShieldPower(
				target->getShield().getShieldPower() -
				shield->getHitRemovePower() * hurtFactor);
		}
	}
}

void PhysicsParticleActionObject::bounceShieldHit(Target *target)
{
	Shield *shield = (Shield *) target->getShield().getCurrentShield()->getAction();
	ShotBounce *shot = (ShotBounce *) info_.data_;
	fixed hurtFactor = shot->getWeapon()->getShieldHurtFactor(*context_);
	if (shield->getShieldType() != Shield::ShieldTypeRoundMag && hurtFactor > 0)
	{
		context_->getActionController().addAction(
			new ShieldHit(target->getPlayerId(), position_, hurtFactor));
	}
}

#ifndef S3D_SERVER

#include <client/ScorchedClient.h>
#include <graph/ParticleEmitter.h>

static void addWallCollisionParticle(Vector &position, ScorchedCollisionId collisionId)
{
	Vector color(1.0f, 1.0f, 1.0f);
	ParticleEmitter emitter;
	emitter.setAttributes(
		8.5f, 8.0f, // Life
		0.2f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		color, 1.0f, // StartColor1
		color, 1.0f, // StartColor2
		color, 0.0f, // EndColor1
		color, 0.0f, // EndColor2
		2.0f, 2.0f, 2.0f, 2.0f, // Start Size
		2.0f, 2.0f, 2.0f, 2.0f, // EndSize
		Vector(0.0f, 0.0f, 0.0f), // Gravity
		false,
		false);

	switch (collisionId)
	{
	case CollisionIdWallLeft:
		emitter.emitWallHit(position,
			ScorchedClient::instance()->getParticleEngine(),
			OptionsTransient::LeftSide);
		break;
	case CollisionIdWallRight:
		emitter.emitWallHit(position,
			ScorchedClient::instance()->getParticleEngine(),
			OptionsTransient::RightSide);
		break;
	case CollisionIdWallTop:
		emitter.emitWallHit(position,
			ScorchedClient::instance()->getParticleEngine(),
			OptionsTransient::TopSide);
		break;
	case CollisionIdWallBottom:
		emitter.emitWallHit(position,
			ScorchedClient::instance()->getParticleEngine(),
			OptionsTransient::BotSide);
		break;
	}
}
#endif

void PhysicsParticleActionObject::shotWallHit(CollisionInfo &collision)
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		addWallCollisionParticle(position_.asVector(), collision.collisionId);
	}
#endif
}
