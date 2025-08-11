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

#include <tankai/TankAISniperGuesser.hpp>
#include <tank/TankLib.hpp>
#include <tanket/Tanket.hpp>
#include <tanket/TanketShotInfo.hpp>
#include <target/TargetLife.hpp>
#include <server/ScorchedServer.hpp>
#include <common/Logger.hpp>
#include <common/RandomGenerator.hpp>

TankAISniperGuesser::TankAISniperGuesser()
{
}

TankAISniperGuesser::~TankAISniperGuesser()
{
}

bool TankAISniperGuesser::guess(Tanket *tanket, Vector &target, 
	float distForSniper, bool checkIntersection, float offset)
{
	fixed angleXYDegs, angleYZDegs, power;
	FileRandomGenerator generator;
	generator.seed(rand());

	FixedVector shotPosition = tanket->getLife().getTargetPosition();
	if (TankLib::getSniperShotTowardsPosition(
		ScorchedServer::instance()->getContext(),
		shotPosition, 
		FixedVector::fromVector(target), 
		fixed::fromFloat(distForSniper), 
		angleXYDegs, angleYZDegs, power,
		checkIntersection))
	{
		fixed xyOffset = generator.getRandFixed("TankAISniperGuesser") * fixed::fromFloat(offset);
		fixed yzOffset = fixed::fromFloat(offset) - xyOffset;

		tanket->getShotInfo().rotateGunXY(angleXYDegs + xyOffset, false);
		tanket->getShotInfo().rotateGunYZ(angleYZDegs + yzOffset, false);
		tanket->getShotInfo().changePower(power, false);

		return true;
	}
	return false;
}
