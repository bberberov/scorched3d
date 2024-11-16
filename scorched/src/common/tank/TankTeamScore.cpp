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

#include <tank/TankTeamScore.h>
#include <common/Defines.h>

TankTeamScore::TankTeamScore() :
	wonGame_(0)
{
	newMatch();
}

TankTeamScore::~TankTeamScore()
{
}

void TankTeamScore::newMatch()
{
	for (int i=1; i<5; i++)
	{
		scores_[i] = 0;
	}
	newGame();
}

void TankTeamScore::newGame()
{
	wonGame_ = 0;
}

void TankTeamScore::addScore(int score, int team)
{
	DIALOG_ASSERT(team > 0 && team < 5);
	scores_[team] += score;
	if (scores_[team] < 0) scores_[team] = 0;
}

int TankTeamScore::getScore(int team)
{
	DIALOG_ASSERT(team > 0 && team < 5);
	return scores_[team];
}

void TankTeamScore::setWonGame(int team)
{
	DIALOG_ASSERT(team > 0 && team < 5);
	wonGame_ = team;
}

int TankTeamScore::getWonGame()
{
	return wonGame_;
}

bool TankTeamScore::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankTeamScore");
	for (int i=1; i<5; i++)
	{
		buffer.addToBufferNamed("score", scores_[i]);
	}
	return true;
}

bool TankTeamScore::readMessage(NetBufferReader &reader)
{
	for (int i=1; i<5; i++)
	{
		if (!reader.getFromBuffer(scores_[i])) return false;
	}
	return true;
}
