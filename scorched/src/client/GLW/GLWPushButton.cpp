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

#include <GLW/GLWPushButton.h>

REGISTER_CLASS_SOURCE(GLWPushButton);

GLWPushButtonI::~GLWPushButtonI()
{

}

GLWPushButton::GLWPushButton(float x, float y, float w, float h, GLWPushButtonI *handler) :
	GLWButton(x, y, w, h)
{
	setHandler(handler);
}

GLWPushButton::~GLWPushButton()
{

}

void GLWPushButton::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (x > x_ && x<x_+w_ &&
		y > y_ && y<y_+h_)
	{
		pressed_ = !pressed_;
		skipRest = true;

		if (pressed_ && handler_)
		{
			handler_->buttonDown(getId());
		}
		else if (handler_)
		{
			((GLWPushButtonI *)handler_)->buttonUp(getId());
		}
	}
}

void GLWPushButton::mouseUp(int button, float x, float y, bool &skipRest)
{

}
