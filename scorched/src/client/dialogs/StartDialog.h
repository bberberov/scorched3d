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

#if !defined(__INCLUDE_StartDialogh_INCLUDE__)
#define __INCLUDE_StartDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <lang/LangString.h>
#include <vector>

class StartDialog : public GLWWindow
{
public:
	static StartDialog *instance();

	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);

protected:
	static StartDialog *instance_;
	struct OptionDefinition
	{
		LangString option;
		const char *description;
		float x, y;
		float width;
	};
	std::vector<OptionDefinition> definitions_;
	int selected_;

private:
	StartDialog();
	virtual ~StartDialog();
};

#endif
