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

#ifndef __BOID2_H
#define __BOID2_H

#include <common/FixedVector.h>
#include <target/TargetState.h>
#include <vector>

class ScorchedContext;
class TargetMovementEntryBoids;
class Target;

class Boid2 : public TargetStateMovement
{
public:  					 
	Boid2(ScorchedContext &context, Target *target, TargetMovementEntryBoids *world); 
	~Boid2();

	void update(fixed frameTime, std::vector<Boid2*> &boidSet, bool complexUpdate);  
	void clearTarget();

protected:
	ScorchedContext &context_;
	Target *target_;
	TargetMovementEntryBoids *world_;

	FixedVector checkGrouping(std::vector<Boid2*> &boidSet);
	bool checkCollision(FixedVector &normal);
	FixedVector &getPosition();
	FixedVector &getVelocity();

	fixed directionMag_;
	FixedVector direction_;

};

#endif /* __BOID2_H */
