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

#include <GLW/GLWCheckBoxText.h>

REGISTER_CLASS_SOURCE(GLWCheckBoxText);

GLWCheckBoxText::GLWCheckBoxText(float x, float y, 
		const LangString &text,
		bool startState,
		float offset) :
	GLWidget(x, y, 0.0f, 20.0f),
	offset_(offset),
	box_(x, y, startState),
	label_(x + box_.getW() + 5.0f - offset_, y - 2.0f - offset_, text)
{
}

GLWCheckBoxText::~GLWCheckBoxText()
{
}

void GLWCheckBoxText::draw()
{
	GLWidget::draw();
	box_.draw();
	label_.draw();
}

void GLWCheckBoxText::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWidget::mouseDown(button, x, y, skipRest);
	box_.mouseDown(button, x, y, skipRest);
	label_.mouseDown(button, x, y, skipRest);
}

void GLWCheckBoxText::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWidget::mouseUp(button, x, y, skipRest);
	box_.mouseUp(button, x, y, skipRest);
	label_.mouseUp(button, x, y, skipRest);
}

void GLWCheckBoxText::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	GLWidget::mouseDrag(button, mx, my, x, y, skipRest);
	box_.mouseDrag(button, mx, my, x, y, skipRest);
	label_.mouseDrag(button, mx, my, x, y, skipRest);
}

float GLWCheckBoxText::getW()
{
	return box_.getW() + 5.0f + label_.getW();
}

void GLWCheckBoxText::setX(float x)
{
	GLWidget::setX(x);
	box_.setX(x);
	label_.setX(x + box_.getW() + 5.0f - offset_);
}

void GLWCheckBoxText::setY(float y)
{
	GLWidget::setY(y);
	box_.setY(y);
	label_.setY(y - 2.0f - offset_);
}
