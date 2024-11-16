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

#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <time.h>
#include <math.h>

OptionsTransient::OptionsTransient(OptionsScorched &optionsGame) :
	optionsGame_(optionsGame), newGame_(false),
	currentRoundNo_(options_, "CurrentRoundNo", 
		"The current number of rounds played in this game", 0, 0),
	currentTurnNo_(options_, "CurrentTurnNo", 
		"The current number of turns played in this round", 0, 1),
	wallType_(options_, "WallType",
		"The current wall type", 0, 0)
{
	
}

OptionsTransient::~OptionsTransient()
{
}

unsigned int OptionsTransient::getLeastUsedTeam(TargetContainer &container)
{
	// Reset all the counts
	std::map<unsigned int, unsigned int> counts;
	for (int i=1; i<=optionsGame_.getTeams(); i++)
	{
		counts[i] = 0;
	}

	// Add all the tanks to the counts
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		container.getTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if (tank->getTeam() > 0 &&
			tank->getState().getTankPlaying())
		{
			counts[tank->getTeam()] ++;
		}
	}

	// Find the least counted team
	unsigned int team = 1;
	unsigned int count = counts[1];
	for (int i=2; i<=optionsGame_.getTeams(); i++)
	{
		if (counts[i] < count)
		{
			team = i;
			count = counts[i];
		}
	}
	return team;
}

bool OptionsTransient::writeToBuffer(NetBuffer &buffer)
{
	if (!OptionEntryHelper::writeToBuffer(options_, buffer, false)) return false;
	return true;
}

bool OptionsTransient::readFromBuffer(NetBufferReader &reader)
{
	if (!OptionEntryHelper::readFromBuffer(options_, reader, false)) return false;
	return true;
}

void OptionsTransient::reset()
{
	currentRoundNo_.setValue(0);
	currentTurnNo_.setValue(1);
}

void OptionsTransient::startNewGame()
{
	currentRoundNo_.setValue(optionsGame_.getNoRounds()+1);
}

void OptionsTransient::newGame()
{
	currentTurnNo_.setValue(1);
	currentRoundNo_.setValue(currentRoundNo_.getValue() + 1);	
	newGameWall();
}

Vector &OptionsTransient::getWallColor()
{
	static Vector wallColor;
	switch (getWallType())
	{
	case wallWrapAround:
		wallColor = Vector(0.5f, 0.5f, 0.0f);
		break;
	case wallBouncy:
		wallColor = Vector(0.0f, 0.0f, 0.5f);
		break;
	case wallConcrete:
		wallColor = Vector(0.5f, 0.5f, 0.5f);
		break;
	default:
		wallColor = Vector(0.0f, 0.0f, 0.0f);
		break;
	}

	return wallColor;
}

void OptionsTransient::newGameWall()
{
	switch (optionsGame_.getWallType())
	{
	case OptionsGame::WallConcrete:
		wallType_.setValue((int) wallConcrete);
		break;
	case OptionsGame::WallBouncy:
		wallType_.setValue((int) wallBouncy);
		break;
	case OptionsGame::WallWrapAround:
		wallType_.setValue((int) wallWrapAround);
		break;
	case OptionsGame::WallNone:
		wallType_.setValue((int) wallNone);
		break;
	float r;
	case OptionsGame::WallActive:	// Bouncy or Wrap
		r = RAND * 2.0f + 1.0f;
		wallType_.setValue((int) r);
		break;
	case OptionsGame::WallInactive:	// Concrete or None
		if (RAND < 0.5f) wallType_.setValue((int) wallConcrete);
		else wallType_.setValue((int) wallNone);
		break;
	default:
		r = RAND * 4.0f;
		wallType_.setValue((int) r);
		break;
	}
}

int OptionsTransient::getArmsLevel()
{
	float start = (float) optionsGame_.getStartArmsLevel();
	float end = (float) optionsGame_.getEndArmsLevel();

	float roundsPlayed = float(getCurrentRoundNo());
	float totalRounds = float(optionsGame_.getNoRounds());

	float armsLevel = start + ((end - start) * (roundsPlayed / totalRounds));
	return (int) armsLevel;
}
