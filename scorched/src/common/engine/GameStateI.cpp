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

#include <engine/GameStateI.h>

std::vector<std::string> GameStateI::perfCounterNames_;

GameStateI::GameStateI(const char *name) :
	gameStateIName_(name)
{

}

GameStateI::~GameStateI()
{

}

void GameStateI::simulate(const unsigned state, float simTime)
{

}

void GameStateI::draw(const unsigned state)
{

}

void GameStateI::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *, int hcount, 
							   bool &skipRest)
{

}

void GameStateI::mouseDown(const unsigned state, GameState::MouseButton button, 
						   int x, int y, bool &skipRest)
{

}

void GameStateI::mouseUp(const unsigned state, GameState::MouseButton button, 
						 int x, int y, bool &skipRest)
{

}

void GameStateI::mouseDrag(const unsigned state, GameState::MouseButton button, 
						   int x, int y, int dx, int dy, bool &skipRest)
{

}

void GameStateI::mouseWheel(const unsigned state, int x, int y, int z, bool &skipRest)
{

}

void GameStateI::enterState(const unsigned state)
{

}

int GameStateI::getPerfCounter(const char *perfName)
{
	for (int i=0; i<(int) perfCounterNames_.size(); i++)
	{
		if (0 == strcmp(perfCounterNames_[i].c_str(), perfName)) return i;
	}

	perfCounterNames_.push_back(perfName);
	return int(perfCounterNames_.size() - 1);
}

void GameStateI::startPerfCount(int counter)
{
	while (int(perfCounters_.size()) <= counter)
	{
		perfCounters_.push_back(new GameStatePerfCounter(perfCounterNames_[perfCounters_.size()].c_str()));
	}
	perfCounters_[counter]->start();
}

void GameStateI::endPerfCount(int counter)
{
	while (int(perfCounters_.size()) <= counter)
	{
		perfCounters_.push_back(new GameStatePerfCounter(perfCounterNames_[perfCounters_.size()].c_str()));
	}
	perfCounters_[counter]->end();
}
