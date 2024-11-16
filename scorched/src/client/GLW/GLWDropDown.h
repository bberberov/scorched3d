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

#if !defined(__INCLUDE_GLWDropDownh_INCLUDE__)
#define __INCLUDE_GLWDropDownh_INCLUDE__

#include <list>
#include <string>
#include <GLW/GLWPushButton.h>
#include <GLW/GLWidget.h>
#include <GLW/GLWSelector.h>

class GLWDropDownI
{
public:
	virtual ~GLWDropDownI();

	virtual void select(unsigned int id, const int pos, GLWSelectorEntry value) = 0;
};

class GLWDropDown : public GLWidget, public GLWSelectorI, public GLWPushButtonI
{
public:
	GLWDropDown(float x = 0.0f, float y = 0.0f, float w = 0.0f);
	virtual ~GLWDropDown();

	void setHandler(GLWDropDownI *handler);
	void addEntry(GLWSelectorEntry text);

	void setCurrentPosition(int pos);
	int getCurrentPosition();
	GLWSelectorEntry *getCurrentEntry();

	// Inherited from GLWidget
	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void setX(float x);
	virtual void setY(float y);

	// Inherited from GLWSelectorI
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
	virtual void noItemSelected();

	// Inhertied from GLWButtonI
	virtual void buttonDown(unsigned int id);
	virtual void buttonUp(unsigned int id);

	void clear();

	REGISTER_CLASS_HEADER(GLWDropDown);
protected:
	std::list<GLWSelectorEntry> texts_;
	GLWSelectorEntry *current_;
	GLWPushButton button_;
	GLWDropDownI *handler_;

};

#endif
