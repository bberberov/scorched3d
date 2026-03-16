////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_GLMenu_hpp_INCLUDE__
#define __INCLUDE_GLMenu_hpp_INCLUDE__

#include <map>
#include <GLW/GLWWindow.hpp>
#include <GLEXT/GLMenuI.hpp>

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
		const LangString& menuName,
		const char*       menuNameInternal,
		const LangString& menuDescription,
		float             width,
		unsigned int      state,
		GLMenuI*          callback,
		Image*            icon  = nullptr,
		unsigned int      flags = 0
	);
	bool         addMenuItem( const char* menuName, const GLMenuItem item );
	GLMenuEntry* getMenu( const char* menuItem );

	virtual void draw();
	virtual void mouseDown( int button, float x, float y, bool& skipRest );
	virtual void mouseUp( int button, float x, float y, bool& skipRest );
	virtual void mouseDrag( int button, float mx, float my, float x, float y, bool& skipRest );
	virtual void keyDown(
		char*                            buffer,
		unsigned int                     keyState,
		KeyboardHistory::HistoryElement* history,
		int                              hisCount,
		bool&                            skipRest
	);

protected:
	std::map< std::string, GLMenuEntry* > menuList_;
};

#endif  // __INCLUDE_GLMenu_hpp_INCLUDE__
