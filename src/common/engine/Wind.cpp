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

#include <engine/Simulator.h>
#include <common/RandomGenerator.h>
#include <common/OptionsScorched.h>
#include <landscapemap/LandscapeMaps.h>

Wind::Wind() : context_(0)
{

}

Wind::~Wind()
{

}

void Wind::newLevel()
{
	RandomGenerator &random =
		context_->getSimulator().getRandomGenerator();

	switch(context_->getOptionsGame().getWindForce())
	{
		case OptionsGame::WindRandom:
			windSpeed_ = (
				(random.getRandFixed("Wind") * fixed(true, 59000)).asInt()); // ie range 0->5
			break;
		case OptionsGame::Wind1:
		case OptionsGame::Wind2:
		case OptionsGame::Wind3:
		case OptionsGame::Wind4:
		case OptionsGame::Wind5:
			windSpeed_ = (
				fixed(int(context_->getOptionsGame().getWindForce()) - 1));
			break;
		case OptionsGame::WindBreezy:
			windSpeed_ = (
				(random.getRandFixed("Wind") * fixed(true, 29000)).asInt());// ie range 0->2);
			break;
		case OptionsGame::WindGale:
			windSpeed_ = (
				(random.getRandFixed("Wind") * fixed(true, 29000)).asInt() + 3); // ie range 3->5);
			break;
		case OptionsGame::WindNone:
		default:
			windSpeed_ = (0);
			break;
	}

	if (windSpeed_ > 0)
	{
		fixed winAngle = random.getRandFixed("Wind") * 360;
		windStartAngle_ = (winAngle);
		windAngle_ = (winAngle);
	}
	else
	{
		windStartAngle_ = (0);
		windAngle_ = (0);
	}

	updateDirection();
	updateChangeTime();
	windStartingDirection_ = windDirection_;
}

void Wind::updateDirection()
{
	if (windSpeed_ > 0)
	{
		fixed windDirX = (windAngle_ / fixed(180) * fixed::XPI).sin();
		fixed windDirY = (windAngle_ / fixed(180) * fixed::XPI).cos();
		FixedVector windDir(windDirX, windDirY, 0);
		windDirection_ = (windDir);
	}
	else
	{
		windDirection_ = FixedVector::getNullVector();
	}
}

void Wind::updateChangeTime()
{
	if (windSpeed_ <= 0) return;
	if (context_->getOptionsGame().getWindType().getValue() ==
		OptionsGame::WindChangeNever) return;

	RandomGenerator &random =
		context_->getSimulator().getRandomGenerator();

	switch (context_->getOptionsGame().getWindType().getValue()) 
	{
	case OptionsGame::WindChangeSomeTimes:
		windChangeTime_ = random.getRandFixed("Wind") * 30 + 30;
		break;
	case OptionsGame::WindChangeFrequently:
		windChangeTime_ = random.getRandFixed("Wind") * 15 + 15;
		break;
	case OptionsGame::WindChangeConstantly:
		windChangeTime_ = random.getRandFixed("Wind") * 5 + 5;
		break;
	case OptionsGame::WindChangeAlways:
		windChangeTime_ = random.getRandFixed("Wind") * 2;
		break;
	}
}

void Wind::simulate(fixed frameTime)
{
	if (windSpeed_ <= 0) return;
	if (context_->getOptionsGame().getWindType().getValue() ==
		OptionsGame::WindChangeNever) return;

	windChangeTime_ -= frameTime;
	if (windChangeTime_ < 0)
	{
		RandomGenerator &random =
			context_->getSimulator().getRandomGenerator();

		fixed winAngle = random.getRandFixed("Wind") * 180 - 90;
		winAngle = fixed(1) - winAngle.cos();
		winAngle *= 25;
		windAngle_ = windStartAngle_ + winAngle;

		updateDirection();
		updateChangeTime();
	}
}
