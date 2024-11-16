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

#include <GLW/GLWDragger.h>
#include <GLEXT/GLState.h>
#include <common/Keyboard.h>
#include <common/ToolTip.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>

REGISTER_CLASS_SOURCE(GLWDragger);

GLWDragger::GLWDragger(float x, float y, float w,  float range) :
	GLWidget(x, y, w, 20.0f), 
	range_(range),
	dragging_(false), handler_(0)
{

}

GLWDragger::~GLWDragger()
{

}

void GLWDragger::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		dragging_ = true;
	}
}

void GLWDragger::mouseUp(int button, float x, float y, bool &skipRest)
{
	dragging_ = false;
}

void GLWDragger::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		float rangeMult = 1.0f;
		unsigned int keyState = 
			Keyboard::instance()->getKeyboardState();
		if (keyState & KMOD_LSHIFT) rangeMult = 0.5f;

		current_ += y /w_ * range_ * rangeMult;
		if (handler_) handler_->currentChanged(getId(), current_);

		skipRest = true;
	}
}
