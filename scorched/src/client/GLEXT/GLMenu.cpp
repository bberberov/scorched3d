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

#include <GLEXT/GLMenu.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLMenuEntry.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>

GLMenu::GLMenu() : GLWWindow("Menu", 0.0f, 10.0f, 10000.0f, 32.0f, eHideName, "")
{
	windowLevel_ = 20000;
}

GLMenu::~GLMenu()
{
}

GLMenuEntry *GLMenu::getMenu(char *menuItem)
{
	std::string menu(menuItem);
	std::map<std::string, GLMenuEntry *>::iterator itor = menuList_.find(menu);
	if (itor != menuList_.end())
	{
		return itor->second;
	}

	return 0;
}

bool GLMenu::addMenu(
	const LangString &menuName,
	char *menuNameInternal, 
	const LangString &menuDescription,
	float width, 
	unsigned int state,
	GLMenuI *callback,
	Image *icon,
	unsigned int flags)
{
	if (getMenu(menuNameInternal)) return false;

	GLMenuEntry *entry = new GLMenuEntry(
		menuName, menuNameInternal, menuDescription,
		width, state, 
		callback, icon, flags);
	menuList_[std::string(menuNameInternal)] = entry;
	return true;
}

bool GLMenu::addMenuItem(char *menuName, const GLMenuItem item)
{
	GLMenuEntry *entry = getMenu(menuName);
	if (!entry) return false;

	entry->addMenuItem((GLMenuItem &) item);
	return true;
}

void GLMenu::draw()
{
	GLState currentDrawState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);

	float currentTop = (float) GLViewPort::getHeight();
	setY(currentTop - h_);
	int x = ScorchedClient::instance()->getGameState().getMouseX();
	int y = ScorchedClient::instance()->getGameState().getMouseY();

	unsigned int currentState =
		ScorchedClient::instance()->getGameState().getState();

	bool selected = false;
	std::map<std::string, GLMenuEntry *>::iterator itor;
	for (itor = menuList_.begin();
		itor != menuList_.end();
		++itor)
	{
		GLMenuEntry *entry = itor->second;
		if (entry->getSelected())
		{
			selected = true;
		}
		else if (entry->inMenu(currentTop, x, y))
		{
			GLWToolTip::instance()->addToolTip(&entry->getToolTip(),
				entry->getX() - 10.0f, entry->getY() - 75, 
				entry->getW(), 75.0f);
		}
	}	

	bool show = true;
	if (OptionsDisplay::instance()->getHideMenus())
	{
		show = (selected || GLWWindowManager::instance()->getFocus(x, y) == getId());
	}
	if (show)
	{
		GLfloat currentWidth = 0.0f;
		std::map<std::string, GLMenuEntry *>::iterator itor;
		for (itor = menuList_.begin();
			itor != menuList_.end();
			++itor)
		{
			GLMenuEntry *entry = itor->second;
			if (!(entry->getFlags() & eMenuAlignRight))
			{
				if (entry->getState() == 0 ||
					entry->getState() == currentState)
				{
					if (entry->getCallback()->getEnabled(entry->getNameInternal()))
					{
						entry->draw(
							currentTop - 1.0f, currentWidth);
						currentWidth += entry->getW() + 1.0f;
					}
				}
			}
		}	

		currentWidth = (float) GLViewPort::getWidth();
		for (itor = menuList_.begin();
			itor != menuList_.end();
			++itor)
		{
			GLMenuEntry *entry = itor->second;
			if (entry->getFlags() & eMenuAlignRight)
			{
				if (entry->getState() == 0 ||
					entry->getState() == currentState)
				{
					if (entry->getCallback()->getEnabled(entry->getNameInternal()))
					{
						currentWidth -= entry->getW() + 1.0f;
						entry->draw(
							currentTop - 1.0f, currentWidth);
					}
				}
			}
		}	
	}
}

void GLMenu::mouseDown(int button, float x, float y, bool &hitMenu)
{
	hitMenu = false;
	float currentTop = (float) GLViewPort::getHeight();

	unsigned int currentState =
		ScorchedClient::instance()->getGameState().getState();

	std::map<std::string, GLMenuEntry *>::iterator itor;
	for (itor = menuList_.begin();
		itor != menuList_.end();
		++itor)
	{
		GLMenuEntry *entry = itor->second;
		if (entry->getState() == 0 ||
			entry->getState() == currentState)
		{
			if (entry->click(currentTop, int(x), int(y)))
			{
				hitMenu = true;
			}
		}
	}
}

void GLMenu::mouseUp(int button, float x, float y, bool &skipRest)
{

}

void GLMenu::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{

}

void GLMenu::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{

}
