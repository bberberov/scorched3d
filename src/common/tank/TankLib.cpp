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

#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankState.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <engine/Simulator.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <GLEXT/GLState.h>
#endif
#include <math.h>

fixed TankLib::getDistanceToTank(FixedVector &position, Target *targetTank)
{
	FixedVector currentdirection = 
		targetTank->getLife().getTargetPosition() - position;
	fixed maxdistance2D = (currentdirection[0] * 
		currentdirection[0] + currentdirection[1] * currentdirection[1]).sqrt();
	return maxdistance2D;
}

void TankLib::getTargetsSortedByDistance(FixedVector &position, 
									   std::list<Target *> &srcTargets,
									  std::list<std::pair<fixed, Target *> > &result,
									  unsigned int teams,
									  fixed maxDistance)
{
	std::list<std::pair<fixed, Target *> > tankDistList;
	std::list<Target *>::iterator itor;
	for (itor = srcTargets.begin();
		itor != srcTargets.end();
		++itor)
	{
		Target *targetTank = (*itor);
		if (targetTank->getAlive())
		{
			if (teams > 0)
			{
				if (targetTank->getType() != Target::TypeTarget)
				{
					if (teams == ((Tanket *) targetTank)->getTeam()) continue;
				}
			}

			fixed maxdistance2D = getDistanceToTank(position, targetTank);
			tankDistList.push_back(std::pair<fixed, Target*>(maxdistance2D, targetTank));
		}
	}

	while (!tankDistList.empty())
	{
		std::list<std::pair<fixed, Target *> >::iterator removeItor = tankDistList.begin();
		std::list<std::pair<fixed, Target *> >::iterator itor = tankDistList.begin(); ++itor;
		for (;itor != tankDistList.end(); ++itor)
		{
			if ((*itor).first <  (*removeItor).first) 
			{
				removeItor = itor;
			}
		}

		if ((*removeItor).first <= maxDistance || maxDistance == -1) result.push_back(*removeItor);
		tankDistList.erase(removeItor);
	}
}

bool TankLib::intersection(ScorchedContext &context,
	FixedVector position, fixed xy, fixed yz, fixed power, 
	int dist)
{
	FixedVector startPosition = position;
	FixedVector velocity = getVelocityVector(xy, yz) * power / fixed(250);

	do
	{
		if (position[2] < context.getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]))
		{
			return true;
		}
		position += velocity;
		velocity += FixedVector(0, 0, fixed(true, -0002));
	} while ((position - startPosition).Magnitude() < dist);

    return false;
}

bool TankLib::getSniperShotTowardsPosition(ScorchedContext &context,
	FixedVector &position, FixedVector &shootAt, fixed distForSniper, 
	fixed &angleXYDegs, fixed &angleYZDegs, fixed &power,
	bool checkIntersection)
{
	// Calculate power
	power = fixed(1000);

	// Calculate direction
	FixedVector direction = shootAt - position;
	fixed angleXYRads = atan2x(direction[1], direction[0]);
	angleXYDegs = (angleXYRads / fixed::XPI) * fixed(180) - fixed(90);

	fixed distance2D = (direction[0] * direction[0] + 
		direction[1] * direction[1]).sqrt();
	fixed angleYZRads = atan2x(distance2D, direction[2]);
	angleYZDegs = fixed(90) - ((angleYZRads /fixed::XPI) * fixed(180));	

	// Special case
	// If we are less than a certain distance and under the position we
	// will use a direct shot on full power
	bool useSniper = ((distance2D < distForSniper) && (shootAt[2] >= position[2] - fixed(2))) ||
		(distForSniper == fixed(-1));
	if (!useSniper) return false;

	// If we check intersection
	if (checkIntersection)
	{
		// Ensure that the sniper shot wont collide with the ground
		// nearer than 10 units from the target
		int allowedIntersectDist = (direction.Magnitude() - fixed(10)).asInt();
		if (intersection(context, position - FixedVector(0, 0, 1), 
			angleXYDegs, angleYZDegs, power, allowedIntersectDist))
		{
			return false;
		}
	}

	return true;
}

void TankLib::getShotTowardsPosition(ScorchedContext &context,
	RandomGenerator &random,
	FixedVector &position, FixedVector &shootAt,
	fixed &angleXYDegs, fixed &angleYZDegs, fixed &power)
{
	// Calculate direction
	FixedVector direction = shootAt - position;
	fixed angleXYRads = atan2x(direction[1], direction[0]);
	angleXYDegs = (angleXYRads / fixed::XPI) * fixed(180) - fixed(90);
	fixed distance2D = (direction[0] * direction[0] + 
		direction[1] * direction[1]).sqrt();

	// Calculate power (very roughly)
	power = fixed(1000);
	if (distance2D < 200)
	{
		power = 750;
		if (distance2D < 100)
		{
			power = 550;
			if (distance2D < 5)
			{
				power = 350;
			}
		}
	}

	// Add some randomness to the power
	power += (random.getRandFixed("TankLib::getShotTowardsPosition") * 200) - 100;
	if (power < 100) power = 100;

	if (context.getSimulator().getWind().getWindOn())
	{
		// Make less adjustments for less wind
		fixed windMag = context.getSimulator().getWind().getWindSpeed() / 5;

		// Try to account for the wind direction
		FixedVector ndirection = direction;
		ndirection[2] = 0;
		ndirection = ndirection.Normalize();
		ndirection = ndirection.get2DPerp();
		fixed windoffsetLR = context.getSimulator().getWind().getWindDirection().dotP(ndirection);
		angleXYDegs += windoffsetLR * distance2D * (fixed(true, 1200) + 
			random.getRandFixed("TankLib::getShotTowardsPosition")) * fixed(true, 400) * windMag;

		fixed windoffsetFB = context.getSimulator().getWind().getWindDirection().dotP(direction.Normalize());
		windoffsetFB /= 10;
		windoffsetFB *= windMag;
		windoffsetFB += 1; // windowoffset FB 0.9 > 1.1

		power *= windoffsetFB;
	}

	// Angle
	angleYZDegs = 45;
}

FixedVector &TankLib::getVelocityVector(fixed xy, fixed yz)
{
	static FixedVector diff;
	diff = FixedVector(
		(-xy / fixed(180) * fixed::XPI).sin() * (yz / fixed(180) * fixed::XPI).cos(),
		(-xy / fixed(180) * fixed::XPI).cos() * (yz / fixed(180) * fixed::XPI).cos(),
		( yz / fixed(180) * fixed::XPI).sin());	
	diff /= 20;
	diff *= fixed(true, 12000);
	return diff;
}

FixedVector &TankLib::getGunPosition(fixed xy, fixed yz)
{
	static FixedVector pos;
	fixed gunLength = 1;
	fixed degToRad = fixed(180) * fixed::XPI;
	pos[0] = gunLength * (xy / degToRad).sin() * 
		((fixed(90) - yz) / degToRad).sin();
	pos[1] = gunLength * (xy / degToRad).cos() * 
		((fixed(90) - yz) / degToRad).sin();
	pos[2] = gunLength * ((fixed(90) - yz) / degToRad).cos();

	return pos;
}

FixedVector &TankLib::getTankGunPosition(FixedVector &tankTurretPosition, fixed xy, fixed yz)
{
	static FixedVector tankGunPosition;
	tankGunPosition = TankLib::getGunPosition(xy, yz);
	tankGunPosition += tankTurretPosition;
	return tankGunPosition;
}
