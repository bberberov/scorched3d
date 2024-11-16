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

#if !defined(__INCLUDE_GLWWindowManagerh_INCLUDE__)
#define __INCLUDE_GLWWindowManagerh_INCLUDE__

#include <map>
#include <deque>
#include <engine/GameStateI.h>
#include <common/KeyboardKey.h>
#include <GLW/GLWWindow.h>
#include <GLEXT/GLMenuI.h>

class GLWWindowManager : 
	public GameStateI,
	public GLMenuI
{
public:
	static GLWWindowManager *instance();

	void addWindow(const unsigned state, GLWWindow *window, 
		KeyboardKey *key = 0, bool visible = false);
	void removeState(unsigned int state);
	void removeWindow(GLWWindow *window);
	bool showWindow(unsigned id);
	bool hideWindow(unsigned id);
	bool windowVisible(unsigned id);
	bool moveToFront(unsigned id);
	bool windowInCurrentState(unsigned id);
	void sortWindowLevels();
	unsigned int getFocus(int x, int y);

	void saveSettings();
	void loadSettings();
	void clear();

	GLWWindow *getWindowByName(const char *name);

	// All inherited from GameStateI
	virtual void enterState(const unsigned state);
	virtual void draw(const unsigned state);
	virtual void simulate(const unsigned state, float simTime);
	virtual void keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest);
	virtual void mouseDown(const unsigned state, GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	virtual void mouseUp(const unsigned state, GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	virtual void mouseDrag(const unsigned state, GameState::MouseButton button, 
		int x, int y, int dx, int dy, bool &skipRest);
	virtual void mouseWheel(const unsigned state, 
		int x, int y, int z, bool &skipRest);

	// Inherited from GLMenuI
	virtual bool getMenuItems(const char* menuName, 
		std::list<GLMenuItem> &result);
	virtual void menuSelection(const char* menuName, 
		const int position, GLMenuItem &item);

protected:
	static GLWWindowManager *instance_;

	// The collection that determines a windows visibility
	std::map<unsigned, bool> windowVisibility_;
	std::map<unsigned, GLWWindow *> idToWindow_;

	// The deque of windows in each state
	struct StateEntry
	{
		unsigned state_;
		std::list<std::pair<KeyboardKey *, GLWWindow *> > windowKeys_;
		std::deque<GLWWindow *> windows_;
	};
	// The collection of states
	std::map<unsigned, StateEntry> stateEntrys_;
	StateEntry *currentStateEntry_;

	// To check for re-entrancy
	unsigned int changeEpoc_;

	void setCurrentEntry(const unsigned state);

private:
	GLWWindowManager();
	virtual ~GLWWindowManager();

};

#endif
