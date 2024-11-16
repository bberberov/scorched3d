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

#if !defined(AFX_GLMENU_H__ED2E8B2C_46B3_400F_A3E2_FC1B53732D15__INCLUDED_)
#define AFX_GLMENU_H__ED2E8B2C_46B3_400F_A3E2_FC1B53732D15__INCLUDED_

#include <map>
#include <GLW/GLWWindow.h>
#include <GLEXT/GLMenuI.h>

class Image;
class GLMenuEntry;
class GLMenu : public GLWWindow
{
public:
	enum MenuFlags
	{
		eMenuAlignRight = 1
	};

	GLMenu();
	virtual ~GLMenu();

	bool addMenu(
		const LangString &menuName,
		char *menuNameInternal, 
		const LangString &menuDescription,
		float width, 
		unsigned int state,
		GLMenuI *callback,
		Image *icon = 0,
		unsigned int flags = 0);
	bool addMenuItem(char *menuName, const GLMenuItem item);
	GLMenuEntry *getMenu(char *menuItem);

	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

protected:
	std::map<std::string, GLMenuEntry *> menuList_;

};

#endif // !defined(AFX_GLMENU_H__ED2E8B2C_46B3_400F_A3E2_FC1B53732D15__INCLUDED_)
