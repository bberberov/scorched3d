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

#include <GLW/GLWScrollButton.h>

GLWScrollButtonI::~GLWScrollButtonI()
{

}

GLWScrollButton::GLWScrollButton(float x, float y, float w, float h) 
	: GLWButton(x, y, w, h), scrollHandler_(0),
	startX_(0.0f), startY_(0.0f)
{

}

GLWScrollButton::~GLWScrollButton()
{

}

void GLWScrollButton::mouseDown(int button, float x, float y, bool &skipRest)
{
	startX_ = 0.0f;
	startY_ = 0.0f;

	if (scrollHandler_)
	{
		scrollHandler_->startDrag(id_);
	}

	GLWButton::mouseDown(button, x, y, skipRest);
}

void GLWScrollButton::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (pressed_)
	{
		if (scrollHandler_)
		{
			startX_ += x;
			startY_ += y;
			scrollHandler_->buttonDrag(id_, startX_, startY_);
			skipRest = true;
		}
	}
}

void GLWScrollButton::setScrollHandler(GLWScrollButtonI *handler)
{
	scrollHandler_ = handler;
}
