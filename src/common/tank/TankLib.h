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

#if !defined(__INCLUDE_TankLibh_INCLUDE__)
#define __INCLUDE_TankLibh_INCLUDE__

#include <engine/ScorchedContext.h>
#include <common/FixedVector.h>
#include <list>

class Tank;
class Target;
class RandomGenerator;
namespace TankLib
{
	bool intersection(ScorchedContext &context,
		FixedVector position, fixed xy, fixed yz, fixed power,
		int dist);
	fixed getDistanceToTank(FixedVector &position, Target *targetTank);
	void getTargetsSortedByDistance(
		FixedVector &position, 
		std::list<Target *> &srcTargets,
		std::list<std::pair<fixed, Target *> > &result,
		unsigned int teams,
		fixed maxDistance = -1);
	bool getSniperShotTowardsPosition(ScorchedContext &context,
		FixedVector &position, FixedVector &shootAt, fixed distForSniper, 
		fixed &angleXYDegs, fixed &angleYZDegs, fixed &power,
		bool checkIntersection = false);
	void getShotTowardsPosition(
		ScorchedContext &context,
		RandomGenerator &random,
		FixedVector &position, FixedVector &shootAt, 
		fixed &angleXYDegs, fixed &angleYZDegs, fixed &power);
	FixedVector &getVelocityVector(fixed xy, fixed yz);
	FixedVector &getGunPosition(fixed xy, fixed yz);
	FixedVector &getTankGunPosition(FixedVector &tankTurretPosition, fixed xy, fixed yz);
};


#endif
