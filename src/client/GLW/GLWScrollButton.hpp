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

#if !defined(__INCLUDE_GLWScrollButtonh_INCLUDE__)
#define __INCLUDE_GLWScrollButtonh_INCLUDE__

#include <GLW/GLWButton.h>

class GLWScrollButtonI
{
public:
	virtual ~GLWScrollButtonI();

	virtual void buttonDrag(unsigned int id, float x, float y) = 0;
	virtual void startDrag(unsigned int id) = 0;
};

class GLWScrollButton : public GLWButton
{
public:
	GLWScrollButton(float x, float y, float w, float h);
	virtual ~GLWScrollButton();

	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	void setScrollHandler(GLWScrollButtonI *handler);

protected:
	GLWScrollButtonI *scrollHandler_;
	float startX_, startY_;
};

#endif
