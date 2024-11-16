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

#if !defined(AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_)
#define AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_

#include <string>
#include <map>
#include <list>
#include <engine/MainLoopI.h>
#include <common/Clock.h>

class GameStateI;
class GameStateStimulusI;
class GameState;

#define GAMESTATE_PERF_COUNTER_START(x, y) { static int __counter__ = x.getPerfCounter(y); x.startPerfCount(__counter__); }
#define GAMESTATE_PERF_COUNTER_END(x, y) { static int __counter__ = x.getPerfCounter(y); x.endPerfCount(__counter__); }

class GameStatePerfCounter
{
public:
	GameStatePerfCounter(const char *name);
	~GameStatePerfCounter();

	void start();
	void end();

	const char *getName() { return name_.c_str(); }
	unsigned int getTotal();
	bool getUsed() { return used_; }

protected:
	bool used_;
	std::string name_;
	unsigned int start_;
	unsigned int total_;
};

class MainLoop;
class GameState : public MainLoopI
{
public:
	enum MouseButton
	{
		MouseButtonLeft = 0x1,
		MouseButtonMiddle = 0x2,
		MouseButtonRight = 0x4,
		MouseButtonLeftDoubleClick = 0x8,
		MouseButtonMiddleDoubleClick = 0x16,
		MouseButtonRightDoubleClick = 0x32
	};

	GameState(MainLoop *mainLoop, const char *name);
	virtual ~GameState();

	// Called by the simulator
	virtual void simulate(float simTime);
	virtual void draw();

	void clear();

	// Called by SDL subsystem 
	void mouseDown(MouseButton button, int x, int y);
	void mouseUp(MouseButton button, int x, int y);
	void mouseMove(int x, int y);
	void mouseWheel(short z);

	// User fns to change + set state
	void setState(const unsigned state);
	void stimulate(const unsigned stimulus);
	void setFakeMiddleButton(bool fake);
	unsigned getState() { return currentState_; }
	int getMouseX() { return currentMouseX_; }
	int getMouseY() { return currentMouseY_; }
	bool &getStateLogging() { return stateLogging_; }
	bool &getStateTimeLogging() { return stateTimeLogging_; }

	int getPerfCounter(const char *name);
	void startPerfCount(int counter);
	void endPerfCount(int counter);

	// User fns to add classes to state management
	void addStateStimulus(const unsigned state, 
						  const unsigned stim, 
						  const unsigned nexts);
	void addStateStimulus(const unsigned state, 
						  GameStateStimulusI *check, 
						  const unsigned nexts);
	void addStateEntry(const unsigned state, 
					   GameStateI *entry);
	void addStateLoop(const unsigned state, 
					  GameStateI *entry, 
					  GameStateI *subEntry);
	void addStateKeyEntry(const unsigned state, 
						  GameStateI *subEntry);
	void addStateMouseDownEntry(const unsigned state, 
								const unsigned buttons, 
								GameStateI *subEntry);
	void addStateMouseUpEntry(const unsigned state, 
							  const unsigned buttons, 
							  GameStateI *subEntry);
	void addStateMouseDragEntry(const unsigned state, 
								const unsigned buttons, 
								GameStateI *subEntry);
	void addStateMouseWheelEntry(const unsigned state, 
								 GameStateI *subEntry);

protected:
	typedef std::list<GameStateI *> StateIList;
	typedef std::pair<GameStateStimulusI *, unsigned> SimulusIPair;
	typedef std::list<SimulusIPair> StiulusIList;

	struct TimerInfo
	{
		GameStateI *gameStateI;
		unsigned int drawTime;
		unsigned int simulateTime;
	};

	struct GameStateSubEntry
	{
		GameStateI *current;
		StateIList subLoopList;
	};

	struct GameStateEntry 
	{
		// Classes called for every loop
		std::list<GameStateSubEntry> loopList; 

		 // Possible stimuli in this state
		std::map<unsigned, unsigned> stimList;
		// Stimili checked every loop
		StiulusIList condStimList; 

		// Classes called on key events
		StateIList subKeyList; 
		// Classes called on mouse down L
		StateIList subMouseDownLeftList; 
		// Classes called on mouse down M
		StateIList subMouseDownMiddleList; 
		// Classes called on mouse down R
		StateIList subMouseDownRightList;
		// Classes called on mouse up L
		StateIList subMouseUpLeftList;
		// Classes called on mouse up M
		StateIList subMouseUpMiddleList; 
		// Classes called on mouse up R
		StateIList subMouseUpRightList;
		// Classes called on mouse drag L
		StateIList subMouseDragLeftList; 
		// Classes called on mouse drag M
		StateIList subMouseDragMiddleList;
		// Classes called on mouse drag R
		StateIList subMouseDragRightList;
		// Classes called on mouse down W
		StateIList subMouseWheelList;
		// Classes called when entering state
		StateIList enterStateList;
	};

	unsigned stateCount_;
	unsigned currentState_; 
	GameStateI *currentStateI_;
	GameStateEntry *currentEntry_; 
	std::map<unsigned, GameStateEntry> stateList_;
	std::string name_;
	bool fakeMiddleButton_;
	bool stateLogging_;
	bool stateTimeLogging_;
	float frameTime_;
	int frameCount_;
	Clock timerClock_;
	Clock overallTimerClock_;
	Clock doubleClickClock_;
	TimerInfo timers_[50];
	MainLoop *mainLoop_;

	// Dragging stuff
	// Up or down for each button (bit field)
	unsigned currentMouseState_; 
	int mouseLDragX_, mouseLDragY_;
	int mouseMDragX_, mouseMDragY_;
	int mouseRDragX_, mouseRDragY_;
	int mouseDoubleX_, mouseDoubleY_;
	int currentMouseX_, currentMouseY_;

	void mouseMoveCall(const unsigned state, MouseButton button, 
		StateIList &currentList, 
		int mx, int my,
		int dx, int dy);
	void mouseUpDown(MouseButton button, bool down, int x, int y);
	GameState::GameStateEntry* 
		getEntry(const unsigned state);
	GameState::GameStateSubEntry* 
		getSubEntry(const unsigned state, GameStateI *entry);
	void clearTimers(bool printTimers = false);

};

#endif // !defined(AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_)
