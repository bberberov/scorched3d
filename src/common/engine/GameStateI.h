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

#if !defined(AFX_GAMESTATEI_H__B6753299_ED54_45EB_A635_733A34F0C920__INCLUDED_)
#define AFX_GAMESTATEI_H__B6753299_ED54_45EB_A635_733A34F0C920__INCLUDED_

#include <engine/GameState.h>
#include <common/KeyboardHistory.h>
#include <vector>

class GameStateI  
{
public:
	GameStateI(const char *name);
	virtual ~GameStateI();

	virtual void simulate(const unsigned state, float simTime);
	virtual void draw(const unsigned state);

	virtual void mouseDown(const unsigned state, GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	virtual void mouseUp(const unsigned state, GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	virtual void mouseDrag(const unsigned state, GameState::MouseButton button,
		int x, int y, int dx, int dy, bool &skipRest);
	virtual void mouseWheel(const unsigned state, int x, int y, int z, bool &skipRest);
	virtual void enterState(const unsigned state);
	virtual void keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest);

	const char *getGameStateIName() { return gameStateIName_; }

	int getPerfCounter(const char *perfName);
	void startPerfCount(int counter);
	void endPerfCount(int counter);

	std::vector<GameStatePerfCounter *> &getPerfCounters() { return perfCounters_; }

protected:
	const char *gameStateIName_;
	std::vector<GameStatePerfCounter *> perfCounters_;

	static std::vector<std::string> perfCounterNames_;
};

#endif // !defined(AFX_GAMESTATEI_H__B6753299_ED54_45EB_A635_733A34F0C920__INCLUDED_)
