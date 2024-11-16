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

#if !defined(__INCLUDE_TargetMovementEntryBoidsh_INCLUDE__)
#define __INCLUDE_TargetMovementEntryBoidsh_INCLUDE__

#include <common/FixedVector.h>
#include <movement/TargetMovementEntry.h>
#include <map>
#include <vector>

class ObjectGroupEntry;
class ObjectGroup;
class Obstacle;
class Boid2;
class TargetMovementEntryBoids : public TargetMovementEntry
{
public:
	TargetMovementEntryBoids();
	virtual ~TargetMovementEntryBoids();

	fixed getCruiseDistance() { return cruiseDistance_; }
	fixed getCruiseVelocity() { return cruiseVelocity_; }
	fixed getMaxVelocity() { return maxVelocity_; }
	fixed getMaxAcceleration() { return maxAcceleration_; }
	FixedVector &getMinBounds() { return minBounds_; }
	FixedVector &getMaxBounds() { return maxBounds_; }

	// Overridden from TargetMovementEntry
	virtual void generate(ScorchedContext &context, 
		RandomGenerator &random, 
		LandscapeMovementType *movementType);
	virtual void simulate(ScorchedContext &context, fixed frameTime);
	virtual void draw();
	virtual void reset();

protected:
	ObjectGroup *objectGroup_;

	unsigned int movementNumber_;
	FixedVector minBounds_, maxBounds_;
	fixed cruiseDistance_;
	fixed maxVelocity_, cruiseVelocity_;
	fixed maxAcceleration_;

	void makeBoids(ScorchedContext &context, RandomGenerator &random,
		FixedVector &maxBounds, FixedVector &minBounds);
	Boid2 *makeBoid(ScorchedContext &context, ObjectGroupEntry *entry);
	void processSet(fixed frameTime, std::vector<Boid2*> &boidSet);
};

#endif // __INCLUDE_TargetMovementEntryBoidsh_INCLUDE__
