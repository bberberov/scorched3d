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

#include <GLW/GLWWindowManager.h>
#include <GLEXT/GLViewPort.h>
#include <image/ImageFactory.h>
#include <dialogs/MainMenuDialog.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <lang/LangResource.h>
#include <XML/XMLFile.h>
#include <limits.h>
#include <set>

GLWWindowManager *GLWWindowManager::instance_ = 0;

GLWWindowManager *GLWWindowManager::instance()
{
	if (!instance_)
	{
		instance_ = new GLWWindowManager;
	}

	return instance_;
}

GLWWindowManager::GLWWindowManager() : 
	GameStateI("GLWWindowManager"),
	currentStateEntry_(0),
	changeEpoc_(0)
{
	setCurrentEntry(UINT_MAX);

	Image *map = new Image(ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/screen.bmp",
		"data/images/screena.bmp",
		false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu(LANG_RESOURCE("WINDOWS", "Windows"), 
			"Windows",
			LANG_RESOURCE("WINDOWS_WINDOW", "Hide and display aspects of the user interface"),
			32.0f, 0, this, map);
}

GLWWindowManager::~GLWWindowManager()
{

}

void GLWWindowManager::clear()
{
	currentStateEntry_ = 0;
	setCurrentEntry(UINT_MAX);
	stateEntrys_.clear();
	idToWindow_.clear();
	windowVisibility_.clear();
}

void GLWWindowManager::setCurrentEntry(const unsigned state)
{
	// Store the currently visible windows
	std::set<unsigned int> shownWindows;
    std::deque<GLWWindow *> windowsCopy;
	std::deque<GLWWindow *>::iterator qitor;

	if (currentStateEntry_)
	{
		windowsCopy = currentStateEntry_->windows_;
		for (qitor = windowsCopy.begin();
			qitor != windowsCopy.end();
			++qitor)
		{
			GLWWindow *window = *qitor;
			if (windowVisible(window->getId()))
			{
				shownWindows.insert(window->getId());
			}
		}
	}

	// Find the next state
	static StateEntry defaultStateEntry;
	defaultStateEntry.state_ = UINT_MAX;

	std::map<unsigned, StateEntry>::iterator itor =
		stateEntrys_.find(state);
	if (itor == stateEntrys_.end())
	{
		currentStateEntry_ = &defaultStateEntry;
	}
	else
	{
		currentStateEntry_ = &(*itor).second;
	}

	// Tell the windows that they have been hidden or shown
	windowsCopy = currentStateEntry_->windows_;
	for (qitor = windowsCopy.begin();
		qitor != windowsCopy.end();
		++qitor)
	{
		GLWWindow *window = *qitor;
		window->windowInit(state);

		// This window should be visible
		if (windowVisible(window->getId()))
		{
			// Only call window display on windows that have not
			// been shown in the last state
			std::set<unsigned int>::iterator shownPrev = 
				shownWindows.find(window->getId());
			if (shownPrev == shownWindows.end())
			{
				window->display();
			}
		}
		else
		{
			// Only call window hide on windows that have
			// been shown in the last state
			std::set<unsigned int>::iterator shownPrev = 
				shownWindows.find(window->getId());
			if (shownPrev != shownWindows.end())
			{
				window->hide();
			}
		}
	}

	sortWindowLevels();
}

void GLWWindowManager::addWindow(const unsigned state, GLWWindow *window, KeyboardKey *key, bool visible)
{
	windowVisibility_[window->getId()] = visible;
	idToWindow_[window->getId()] = window;

	stateEntrys_[state].windows_.push_back(window);
	stateEntrys_[state].state_ = state;
	stateEntrys_[state].windowKeys_.push_back(
		std::pair<KeyboardKey *, GLWWindow *>(key, window));
}

void GLWWindowManager::removeWindow(GLWWindow *removeWindow)
{
	windowVisibility_.erase(removeWindow->getId());
	idToWindow_.erase(removeWindow->getId());
	std::map<unsigned, StateEntry>::iterator itor;
	for (itor = stateEntrys_.begin();
		itor != stateEntrys_.end();
		++itor)
	{
		{
			std::list<std::pair<KeyboardKey *, GLWWindow *> > tmpList;
			std::list<std::pair<KeyboardKey *, GLWWindow *> >::iterator keyItor;
			for (keyItor = itor->second.windowKeys_.begin();
				keyItor != itor->second.windowKeys_.end();
				++keyItor)
			{
				if (keyItor->second != removeWindow) 
				{
					tmpList.push_back(*keyItor);
				}
			}
			itor->second.windowKeys_ = tmpList;
		}

		{
			std::deque<GLWWindow *> tmpList;
			while (!itor->second.windows_.empty())
			{
				GLWWindow *tmpWindow = itor->second.windows_.front();
				itor->second.windows_.pop_front();
				if (removeWindow != tmpWindow) tmpList.push_back(tmpWindow);
			}
			itor->second.windows_ = tmpList;
		}
	}
}

void GLWWindowManager::removeState(unsigned int state)
{
	stateEntrys_.erase(state);

	static StateEntry defaultStateEntry;
	defaultStateEntry.state_ = UINT_MAX;
	currentStateEntry_ = &defaultStateEntry;
}

bool GLWWindowManager::showWindow(unsigned id)
{
	if (!windowInCurrentState(id)) return false;

	std::map<unsigned, bool>::iterator itor =
		windowVisibility_.find(id);
	if (itor != windowVisibility_.end())
	{
		if (!(*itor).second)
		{
			GLWWindow *window = idToWindow_[id];
			(*itor).second = true;
			window->display();
			moveToFront(id);

			changeEpoc_++;
			return true;
		}
	}

	return false;
}

bool GLWWindowManager::moveToFront(unsigned id)
{
	GLWWindow *found = 0;
	std::deque<GLWWindow *> tmpList;

	while (!currentStateEntry_->windows_.empty())
	{
		GLWWindow *window = currentStateEntry_->windows_.front();
		currentStateEntry_->windows_.pop_front();
		if (window->getId() == id) found = window;
		else tmpList.push_back(window);
	}

	currentStateEntry_->windows_ = tmpList;
	if (found) 
	{
		changeEpoc_++;
		currentStateEntry_->windows_.push_back(found);
		sortWindowLevels();
	}

	return (found != 0);
}

void GLWWindowManager::sortWindowLevels()
{
	if (!currentStateEntry_) return;

	std::deque<GLWWindow *> &windows = currentStateEntry_->windows_;

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i=0; i<int(windows.size())-1; i++)
		{
			GLWWindow *first = windows[i];
			GLWWindow *second = windows[i+1];
			if (first->getWindowLevel() < second->getWindowLevel())
			{
				windows[i] = second;
				windows[i+1] = first;
				changed = true;
				changeEpoc_++;
			}
		}
	}
}

bool GLWWindowManager::windowVisible(unsigned id)
{
	std::map<unsigned, bool>::iterator itor =
		windowVisibility_.find(id);
	if (itor != windowVisibility_.end())
	{
		return ((*itor).second);
	}

	return false;
}

bool GLWWindowManager::hideWindow(unsigned id)
{
	std::map<unsigned, bool>::iterator itor =
		windowVisibility_.find(id);
	if (itor != windowVisibility_.end())
	{
		if ((*itor).second)
		{
			GLWWindow *window = idToWindow_[id];
			window->hide();

			(*itor).second = false;
			changeEpoc_++;
			return true;
		}
	}

	return false;
}

bool GLWWindowManager::windowInCurrentState(unsigned id)
{
	if (!currentStateEntry_) return false;

	std::deque<GLWWindow *>::iterator itor;
	for (itor = currentStateEntry_->windows_.begin();
		itor != currentStateEntry_->windows_.end();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (window->getId() == id)
		{
			return true;
		}
	}
	return false;
}

void GLWWindowManager::enterState(const unsigned state)
{
	if (currentStateEntry_ &&
		currentStateEntry_->state_ != UINT_MAX &&
		currentStateEntry_->state_ != state) setCurrentEntry(state);
}

void GLWWindowManager::draw(const unsigned state)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::iterator itor;
	for (itor = currentStateEntry_->windows_.begin();
		itor != currentStateEntry_->windows_.end();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->draw();
			if (entryEpoc != changeEpoc_) break;
		}
	}
}

GLWWindow *GLWWindowManager::getWindowByName(const char *name)
{
	if (!currentStateEntry_) return 0;

	std::deque<GLWWindow *>::iterator itor;
	for (itor = currentStateEntry_->windows_.begin();
		itor != currentStateEntry_->windows_.end();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (0 == strcmp(window->getName(), name)) return window;
	}
	return 0;
}

unsigned int GLWWindowManager::getFocus(int x, int y)
{
	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			if (GLWidget::inBox(
				(float) x, (float) y,
				window->getX(), window->getY(),
				window->getW(), window->getH()))
			{
				return window->getId();
			}
		}
	}
	return 0;
}

void GLWWindowManager::simulate(const unsigned state, float simTime)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::iterator itor;
	for (itor = currentStateEntry_->windows_.begin();
		itor != currentStateEntry_->windows_.end();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->simulate(simTime);
			if (entryEpoc != changeEpoc_) break;
		}
	}	
}

void GLWWindowManager::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->keyDown(buffer, keyState,
								history, hisCount, 
								skipRest);
			if (skipRest) break;
			if (entryEpoc != changeEpoc_) break;
		}
	}

	if (skipRest) return;

	for (int i=0; i<hisCount; i++)
	{
		std::list<std::pair<KeyboardKey *, GLWWindow *> >::iterator keyItor;
		std::list<std::pair<KeyboardKey *, GLWWindow *> >::iterator endKeyItor = 
			currentStateEntry_->windowKeys_.end();
		for (keyItor = currentStateEntry_->windowKeys_.begin();
			keyItor != endKeyItor;
			++keyItor)
		{
			KeyboardKey *key = (*keyItor).first;
			if (key && key->keyDown(buffer, keyState))
			{
				GLWWindow *window = (*keyItor).second;
				if (windowVisible(window->getId()))
				{
					hideWindow(window->getId());
				}
				else
				{
					showWindow(window->getId());
				}
			}
		}
	}
}

void GLWWindowManager::mouseDown(const unsigned state, GameState::MouseButton button, int x, int y, bool &skipRest)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->mouseDown((int) button, (float) x, (float) y, skipRest);
			if (skipRest) break;
			if (entryEpoc != changeEpoc_) break;
		}
	}
}

void GLWWindowManager::mouseUp(const unsigned state, GameState::MouseButton button, int x, int y, bool &skipRest)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->mouseUp((int) button, (float) x, (float) y, skipRest);
			if (skipRest) break;
			if (entryEpoc != changeEpoc_) break;
		}
	}
}

void GLWWindowManager::mouseDrag(const unsigned state, GameState::MouseButton button, 
							  int x, int y, int dx, int dy, bool &skipRest)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->mouseDrag((int) button, (float) x, (float) y, (float) dx, (float) dy, skipRest);
			if (skipRest) break;
			if (entryEpoc != changeEpoc_) break;
		}
	}
}

void GLWWindowManager::mouseWheel(const unsigned state, 
	int x, int y, int z, bool &skipRest)
{
	if (currentStateEntry_->state_ != state) setCurrentEntry(state);
	unsigned int entryEpoc = changeEpoc_;

	std::deque<GLWWindow *>::reverse_iterator itor;
	for (itor = currentStateEntry_->windows_.rbegin();
		itor != currentStateEntry_->windows_.rend();
		++itor)
	{
		GLWWindow *window = (*itor);
		if (windowVisible(window->getId()))
		{
			window->mouseWheel((float) x, (float) y, (float) z, skipRest);
			if (skipRest) break;
			if (entryEpoc != changeEpoc_) break;
		}
	}
}

bool GLWWindowManager::getMenuItems(const char* menuName, 
								 std::list<GLMenuItem> &items)
{
	if (currentStateEntry_) 
	{
		std::map<unsigned, GLWWindow *>::iterator itor;
		for (itor = idToWindow_.begin();
			itor != idToWindow_.end();
			++itor)
		{
			unsigned id = (*itor).first;
			GLWWindow *window = (*itor).second;

			if (!(window->getWindowState() & GLWWindow::eHideName) &&
				windowInCurrentState(id))
			{
				items.push_back(
					GLMenuItem(
						LANG_RESOURCE(window->getName(), window->getName()), 
						&window->getToolTip(), 
						windowVisible(window->getId())));
			}
		}
	}
	return true;
}

void GLWWindowManager::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	if (currentStateEntry_) 
	{
		int pos = 0;
		std::map<unsigned, GLWWindow *>::iterator itor;
		for (itor = idToWindow_.begin();
			itor != idToWindow_.end();
			++itor)
		{
			unsigned id = (*itor).first;
			GLWWindow *window = (*itor).second;

			if (!(window->getWindowState() & GLWWindow::eHideName) &&
				windowInCurrentState(id))
			{
				if (pos++ == position) 
				{
					if (windowVisible(window->getId()))
					{
						hideWindow(window->getId());
					}
					else
					{
						showWindow(window->getId());
					}
					return;
				}
			}
		}
	}
}

void GLWWindowManager::loadSettings()
{
	XMLFile file;
	std::string fileName = S3D::getSettingsFile("windowsettings.xml");
	if (!file.readFile(fileName))
	{
		S3D::dialogMessage("GLWWindowManager", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  fileName.c_str(),
					  file.getParserError()));
		return;
	}
	if (!file.getRootNode()) return;

	// Get root nodes
	XMLNode *display = 0, *settings = 0;
	if (!file.getRootNode()->getNamedChild("display", display)) return;
	if (!file.getRootNode()->getNamedChild("settings", settings)) return;

	// Get display size
	int width, height;
	if (!display->getNamedChild("width", width)) return;
	if (!display->getNamedChild("height", height)) return;

	// Check display size is the same as the current size
	// if not then discard the windows size changes
	bool resetPositions = 
		(width != GLViewPort::getWidth() ||
		height != GLViewPort::getHeight());

	// Itterate all of the positions in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = settings->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        ++childrenItor)
    {
		XMLNode *node = (*childrenItor);

		// Get the window position
		std::string window;
		if (!node->getNamedChild("window", window)) return;

		// Set the new position
		std::map<unsigned, GLWWindow *>::iterator winitor;
		for (winitor = idToWindow_.begin();
			winitor != idToWindow_.end();
			++winitor)
		{
			GLWWindow *w = (*winitor).second;
			if (0 == strcmp(w->getName(), window.c_str()))
			{
				w->loadSettings(node, resetPositions);
				break;
			}
		}
	}
}

void GLWWindowManager::saveSettings()
{
	XMLNode node("settings");

	XMLNode *display = new XMLNode("display");
	XMLNode *settings = new XMLNode("settings");
	node.addChild(display);
	node.addChild(settings);

	// Add display
	display->addChild(new XMLNode("width", GLViewPort::getWidth()));
	display->addChild(new XMLNode("height", GLViewPort::getHeight()));

	if (OptionsDisplay::instance()->getSaveWindowPositions())
	{
		// Add positions
		std::map<unsigned, GLWWindow *>::iterator winitor;
		for (winitor = idToWindow_.begin();
			winitor != idToWindow_.end();
			++winitor)
		{
			GLWWindow *w = (*winitor).second;

			XMLNode *setting = new XMLNode("setting");
			settings->addChild(setting);
			setting->addChild(new XMLNode("window", w->getName()));
			w->saveSettings(setting);
		}
	}

	std::string fileName = S3D::getSettingsFile("windowsettings.xml");
	node.writeToFile(fileName);
}
