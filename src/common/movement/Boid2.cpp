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

#include <movement/Boid2.h>
#include <movement/TargetMovementEntryBoids.h>
#include <common/OptionsScorched.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <target/Target.h>
#include <target/TargetLife.h>

Boid2::Boid2(ScorchedContext &context, Target *target, TargetMovementEntryBoids *world) :
	context_(context), target_(target), world_(world)
{
}

Boid2::~Boid2()
{
}

FixedVector &Boid2::getPosition()
{
	static FixedVector result;
	if (target_) return target_->getLife().getTargetPosition();
	return result;
}

FixedVector &Boid2::getVelocity()
{
	static FixedVector result;
	if (target_) return target_->getLife().getVelocity();
	return result;
}

void Boid2::clearTarget()
{
	target_ = 0;
}

void Boid2::update(fixed frameTime, std::vector<Boid2*> &boidSet, bool complexUpdate)
{
	// Limit Velocity
	directionMag_ = getVelocity().Magnitude();
	direction_ = getVelocity() / directionMag_;
	if (directionMag_ > world_->getMaxVelocity()) 
	{
		getVelocity() = direction_ * world_->getMaxVelocity();
		directionMag_ = world_->getMaxVelocity();
	}

	// Update position and rotation
	if (target_)
	{
		fixed yaw = atan2x(getVelocity()[0], getVelocity()[1]);
		fixed newRotation = -fixed(yaw) / fixed::XPI * 180;

		FixedVector newPosition = getPosition() + getVelocity();
		target_->getLife().setTargetPositionAndRotation(newPosition, newRotation);
	}

	// Calculate new velocity
	FixedVector collisionNormal;
	FixedVector newVelocity;
	if (checkCollision(collisionNormal))
	{
		// Collision
		newVelocity = collisionNormal / 10;
	}
	else
	{
		if (complexUpdate)
		{
			newVelocity = checkGrouping(boidSet);
		}

		// Wander
		{
			fixed cruiseDiff = world_->getCruiseVelocity() - directionMag_;
			fixed cruiseChange = cruiseDiff / 10;
			newVelocity += direction_ * cruiseDiff;
		}

		// Level out the flight
		{
			newVelocity[2] -= getVelocity()[2] / 100;
		}
	}

	// Limit acceleration
	fixed velocityMag = newVelocity.Magnitude();
	if (velocityMag > world_->getMaxAcceleration())
	{
		newVelocity.StoreNormalize();
		newVelocity *= world_->getMaxAcceleration();
	}

	if (target_)
	{
		newVelocity += getVelocity();
		target_->getLife().setVelocity(newVelocity);

		if (complexUpdate)
		{
			if (context_.getOptionsGame().getActionSyncCheck() &&
				context_.getOptionsGame().getActionMovementSyncCheck())
			{
				context_.getSimulator().addSyncCheck(
					S3D::formatStringBuffer("TargetMovement: %u %s %s", 
						target_->getPlayerId(),
						target_->getLife().getTargetPosition().asQuickString(),
						target_->getLife().getVelocity().asQuickString()));
			}
		}
	}
}

FixedVector Boid2::checkGrouping(std::vector<Boid2*> &boidSet)
{
	FixedVector posMatch, velMatch, velDist;
	FixedVector normalizedVelocity = direction_;

	int count = 0, countDist = 0;
	std::vector<Boid2*>::iterator itor;
	for (itor = boidSet.begin(); itor != boidSet.end(); ++itor)
	{
		Boid2 *other = (Boid2 *) *itor;
		if (!other || other == this) continue; 

		FixedVector direction = getPosition() - other->getPosition();
		fixed distance = direction.Magnitude();
		FixedVector normalizedDirection = direction / distance;
		fixed dp = normalizedDirection.dotP(normalizedVelocity);
		if (dp.abs() < fixed(true, 3000))
		{
			count++;

			posMatch += other->getPosition();
			velMatch += other->getVelocity();

			if (distance < world_->getCruiseDistance())
			{
				velDist += direction;
				countDist++;
			}
		}
	}

	if (countDist > 0)
	{
		velDist /= countDist;
		velDist /= 10;
	}

	if (count > 0)
	{
		posMatch /= count;
		posMatch = (posMatch - getPosition()) / 10;

		velMatch /= count;
		velMatch = (velMatch - getVelocity()) / 10;
	}

	FixedVector velocity;
	velocity += posMatch;
	velocity += velMatch;
	velocity += velDist;
	return velocity;
}

bool Boid2::checkCollision(FixedVector &normal)
{
	int collision = 0;
	FixedVector &position = getPosition();
	if (position[0] < world_->getMinBounds()[0])
	{
		collision++;
		normal += FixedVector(1, 0, 0);
	}
	else if (position[0] > world_->getMaxBounds()[0])
	{
		collision++;
		normal += FixedVector(-1, 0, 0);
	}

	if (position[1] < world_->getMinBounds()[1])
	{
		collision++;
		normal += FixedVector(0, 1, 0);
	}
	else if (position[1] > world_->getMaxBounds()[1])
	{
		collision++;
		normal += FixedVector(0, -1, 0);
	}

	if (position[2] < world_->getMinBounds()[2])
	{
		collision++;
		normal += FixedVector(0, 0, 1);
	}
	else if (position[2] > world_->getMaxBounds()[2])
	{
		collision++;
		normal += FixedVector(0, 0, -1);
	}

	for (int i=0; i<4; i++)
	{
		fixed dist = fixed(i) * 3;
		FixedVector newPosition = position + direction_ * dist;

		if (context_.getLandscapeMaps().getGroundMaps().getHeight(
			newPosition[0].asInt(), newPosition[1].asInt()) > newPosition[2] - fixed(5))
		{
			collision++;
			normal += context_.getLandscapeMaps().getGroundMaps().getNormal(
				newPosition[0].asInt(), newPosition[1].asInt());
			break;
		}
	}

	if (collision > 0)
	{
		normal.StoreNormalize();
		return true;
	}

	return false;
}
