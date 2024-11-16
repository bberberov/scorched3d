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

#include <GLW/GLWSlider.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLState.h>
#include <common/Keyboard.h>
#include <common/ToolTip.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>

REGISTER_CLASS_SOURCE(GLWSlider);

GLWSlider::GLWSlider(float x, float y, float w, 
	float min, float max, int marks) :
	GLWidget(x, y, w, 20.0f), 
	min_(min), max_(max),
	marks_(marks),
	handler_(0)
{
	setCurrent(min_);
}

GLWSlider::~GLWSlider()
{

}

void GLWSlider::draw()
{
	glBegin(GL_LINE_LOOP);
		drawBox(x_ - 4.0f, y_ + 8.0f, w_ + 12.0f, 4.0f, false);
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	for (int i=0; i<=marks_; i++)
	{
		float dist = float(i) / float(marks_);
		glVertex2f(x_ + w_ * dist, y_ + 12.0f);
		glVertex2f(x_ + w_ * dist, y_ + 18.0f);

		glVertex2f(x_ + w_ * dist, y_ + 2.0f);
		glVertex2f(x_ + w_ * dist, y_ + 8.0f);
	}
	glEnd();

	float dist = (current_ - min_) / max_;
	glColor3f(0.8f, 0.8f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(x_ + w_ * dist , y_);
		glVertex2f(x_ + w_ * dist + 4.0f, y_);
		glVertex2f(x_ + w_ * dist + 4.0f, y_ + 20.0f);
		glVertex2f(x_ + w_ * dist, y_ + 20.0f);
	glEnd();
	glBegin(GL_LINE_LOOP);
		drawBox(x_ + w_ * dist, y_, 4.0f, 20.0f, true);
	glEnd();
}

void GLWSlider::setCurrent(float current)
{ 
	current_ = current; 
	if (handler_) handler_->currentChanged(getId(), current_);
}

void GLWSlider::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		float dist = (x - x_) / w_;
		if (dist >= 0.0f && dist <= 1.0f)
		{
			setCurrent(((max_ - min_) * dist) + min_);
		}
	}
}

void GLWSlider::mouseUp(int button, float x, float y, bool &skipRest)
{
}

void GLWSlider::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	mouseDown(button, mx, my, skipRest);
}
