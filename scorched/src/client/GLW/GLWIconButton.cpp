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

#include <GLW/GLWIconButton.h>

REGISTER_CLASS_SOURCE(GLWIconButton);

GLWIconButton::GLWIconButton(
	float x , float y, 
	float w, float h, 
	GLWButtonI *handler,
	unsigned flags) :
	GLWButton(x, y, w, h, handler, flags),
	icon_(x, y, w, h)
{
}

GLWIconButton::~GLWIconButton()
{
}

void GLWIconButton::setToolTip(ToolTip *tooltip)
{
	GLWButton::setToolTip(tooltip);
	icon_.setToolTip(tooltip);
}

void GLWIconButton::setX(float x)
{
	GLWButton::setX(x);
	icon_.setX(x);
}

void GLWIconButton::setY(float y)
{
	GLWButton::setY(y);
	icon_.setY(y);
}

void GLWIconButton::setW(float w)
{
	GLWButton::setW(w);
	icon_.setW(w);
}

void GLWIconButton::setH(float h)
{
	GLWButton::setH(h);
	icon_.setH(h);
}

void GLWIconButton::draw()
{
	GLWidget::draw();

	if (pressed_)
	{
		icon_.setX(x_ + 2.0f);
		icon_.setY(y_ - 2.0f);
	}
	else
	{
		icon_.setX(x_);
		icon_.setY(y_);
	}

	icon_.draw();
}
