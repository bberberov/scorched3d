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

#include <GLW/GLWCheckBox.h>
#include <GLEXT/GLState.h>

GLWCheckBoxI::~GLWCheckBoxI()
{

}

REGISTER_CLASS_SOURCE(GLWCheckBox);

GLWCheckBox::GLWCheckBox(float x, float y, bool startState) :
	GLWidget(x, y, 20.0f, 20.0f), state_(startState), handler_(0)
{
}

GLWCheckBox::~GLWCheckBox()
{
}

void GLWCheckBox::setHandler(GLWCheckBoxI *handler)
{
	handler_ = handler;
}

void GLWCheckBox::draw()
{
	float halfW = h_ / 2.0f;
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, halfW, false);
	glEnd();

	if (state_)
	{
		// Draw check mark
		glColor3f(0.2f, 0.2f, 0.2f);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x_ + halfW, y_ + halfW);
			drawCircle(16, -1, x_ + halfW, y_ + halfW, halfW - 3.0f);
		glEnd();
	}
	GLWidget::draw();
}

void GLWCheckBox::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (x > x_ && x<x_+w_ &&
		y > y_ && y<y_+h_)
	{
		state_ = !state_;
		skipRest = true;
		if (handler_) handler_->stateChange(state_, getId());
	}
	GLWidget::mouseDown(button, x, y, skipRest);
}
