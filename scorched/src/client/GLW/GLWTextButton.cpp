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

#include <GLW/GLWTextButton.h>
#include <GLW/GLWFont.h>

REGISTER_CLASS_SOURCE(GLWTextButton);

GLWTextButton::GLWTextButton(const LangString &buttonText,
	float x, float y, float w, GLWButtonI *handler,
	unsigned flags, float size) :
	GLWButton(x, y, w, size + 8.0f, handler, flags), 
	label_(x, y -2, buttonText, size),
	color_(GLWFont::widgetFontColor)
{
	setName(LangStringUtil::convertFromLang(buttonText));
}

GLWTextButton::~GLWTextButton()
{
}

void GLWTextButton::draw()
{
	GLWButton::draw();

	float x = x_;
	if (!pressed_)
	{
		label_.setY(y_ - 2);
	}
	else
	{
		x = x_ + 1;
		label_.setY(y_ - 3);
	}
	if (flags_ & ButtonFlagCenterX)
	{
		label_.calcWidth();
		x += (w_ / 2.0f) - (label_.getW() / 2.0f);
	}

	if (enabled_) label_.setColor(color_);
	else label_.setColor(GLWFont::disabledWidgetFontColor);

	label_.setX(x);
	label_.draw();
}
