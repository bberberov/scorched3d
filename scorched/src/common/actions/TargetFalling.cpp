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

#include <actions/TargetFalling.h>
#include <target/TargetDamage.h>
#include <target/TargetContainer.h>
#include <target/TargetState.h>
#include <target/TargetParachute.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <tanket/Tanket.h>
#include <tanket/TanketAccessories.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Parachute.h>
#include <common/OptionsScorched.h>
#include <landscapemap/DeformLandscape.h>

TargetFalling::TargetFalling(Weapon *weapon, unsigned int fallingPlayerId,
				   WeaponFireContext &weaponContext,
				   Parachute *parachute) :
	PhysicsParticle(weaponContext.getInternalContext().getReferenced()),
	weapon_(weapon),
	fallingPlayerId_(fallingPlayerId),
	weaponContext_(weaponContext), parachute_(parachute)
{
}

TargetFalling::~TargetFalling()
{
	if (context_)
	{
		Target *target = context_->getTargetContainer().getTargetById(fallingPlayerId_);
		if (target)
		{
			if (target->getTargetState().getFalling() == this)
			{
				target->getTargetState().setFalling(0);
			}
		}
	}
}

void TargetFalling::init()
{
	Target *current = 
		context_->getTargetContainer().getTargetById(fallingPlayerId_);
	if (current && 
		!current->getTargetState().getFalling() && 
		!current->getTargetState().getNoFalling())
	{
		current->getTargetState().setFalling(this);

		// Store the start positions
		tankStartPosition_ = current->getLife().getTargetPosition();

		FixedVector velocity(0, 0, 0);
		PhysicsParticleInfo info(ParticleTypeFalling, fallingPlayerId_, this);
		getPhysics().setPhysics(info, *context_, tankStartPosition_, velocity);
		getPhysics().setForces(0, 1);
	}
	else
	{
		collision_ = true;
	}
}

std::string TargetFalling::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s",
		fallingPlayerId_, weapon_->getParent()->getName());
}

void TargetFalling::simulate(fixed frameTime, bool &remove)
{
	if (!collision_)
	{
		// Slow falling
		if (parachute_)	getPhysics().applyForce(parachute_->getSlowForce());

		// Move the tank to the new position
		Target *target = context_->getTargetContainer().getTargetById(fallingPlayerId_);
		if (target && target->getAlive())
		{
			FixedVector &position = getPhysics().getPosition();
			if (position[0] != 0 || position[1] != 0 || position[2] != 0)
			{
				target->getLife().setTargetPosition(position);
			}
		}
		else collision_ = true;
	}

	PhysicsParticle::simulate(frameTime, remove);
}

void TargetFalling::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	Target *current = context_->getTargetContainer().getTargetById(fallingPlayerId_);
	if (current && current->getAlive())
	{
		// Find how far we have falled to get the total damage
		fixed dist = (tankStartPosition_ - position.getPosition()).Magnitude();
		fixed damage = dist * 20;

		// Check we need to cancel the damage
		fixed minDist = fixed(context_->getOptionsGame().
			getMinFallingDistance()) / 10;
		if (dist < minDist)
		{
			// No damage (or parachutes used for tiny falls)
			damage = 0;
		}
		else if (!context_->getOptionsGame().getTankFallingDamage() && current->getType() != Target::TypeTarget) 
		{
			damage = 0;
		}
		else if (current->getTargetState().getNoFallingDamage())
		{
			damage = 0;
		}
		if (parachute_)
		{
			fixed ParachuteThreshold = 0;
			if (dist >= ParachuteThreshold)
			{
				// No damage we were using parachutes
				damage = 0;

				// Remove parachutes if we have one
				if (current->getType() != Target::TypeTarget)
				{
					Tanket *currentTanket = (Tanket *) current;
					currentTanket->getAccessories().rm(parachute_->getParent(),
						parachute_->getParent()->getUseNumber());
					if (!currentTanket->getAccessories().canUse(parachute_->getParent()))
					{
						currentTanket->getParachute().setCurrentParachute(0);
					}
				}
			}
		}

		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("TargetFalling: %u %s", 
					current->getPlayerId(),
					position.getPosition().asQuickString()));
		}

		// Move the tank to the final position
		current->getLife().setTargetPosition(position.getPosition());

		// Flatten the area around tanks
		if (current->getType() == Target::TypeTank)
		{
			DeformLandscape::flattenArea(*context_, position.getPosition());
		}

		// Check if we have collected/given any items
		std::map<unsigned int, Target *> collisionTargets;
		context_->getTargetSpace().getCollisionSet(
			current->getLife().getTargetPosition(), 3, collisionTargets, false);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			++itor)
		{
			Target *collisionTarget = (*itor).second;

			if (current->getType() != Target::TypeTank &&
				current->getTargetState().getDriveOverToDestroy() &&
				collisionTarget->getType() != Target::TypeTarget)
			{
				// Kill the falling target
				WeaponFireContext weaponContext(weaponContext_);
				weaponContext.setPlayerId(collisionTarget->getPlayerId());

				TargetDamage::damageTarget(*context_,
					weapon_, fallingPlayerId_, weaponContext, 
					current->getLife().getLife(),
					false, false, false);

				// This removes the target so no point continuing
				break;
			}
			else if (collisionTarget->getType() != Target::TypeTank &&
				collisionTarget->getTargetState().getDriveOverToDestroy() &&
				current->getType() != Target::TypeTarget)
			{
				// Kill the target we've fallen on
				WeaponFireContext weaponContext(weaponContext_);
				weaponContext.setPlayerId(current->getPlayerId());

				TargetDamage::damageTarget(*context_, weapon_, 
					collisionTarget->getPlayerId(), weaponContext_, 
					collisionTarget->getLife().getLife(),
					false, false, false);
			}
		}

		// Add the damage to the tank
		// Note: This may remove the target so do this last
		TargetDamageCalc::damageTarget(
			*context_,
			fallingPlayerId_, weapon_, 
			weaponContext_, damage, 
			false, false, false);
	}

	PhysicsParticle::collision(position, collisionId);
}
