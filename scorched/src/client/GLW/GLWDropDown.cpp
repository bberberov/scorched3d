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

#include <GLW/GLWDropDown.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWToolTip.h>
#include <client/ScorchedClient.h>

GLWDropDownI::~GLWDropDownI()
{

}

REGISTER_CLASS_SOURCE(GLWDropDown);

GLWDropDown::GLWDropDown(float x, float y, float w) :
	GLWidget(x, y, w, 25.0f), 
	button_(x + w - 22.0f, y + 2.0f, 20.0f, 21.0f),
	handler_(0), current_(0)
{
	button_.setHandler(this);
}

GLWDropDown::~GLWDropDown()
{

}

void GLWDropDown::setHandler(GLWDropDownI *handler)
{
	handler_ = handler;
}

void GLWDropDown::clear()
{
	current_ = 0;
	texts_.clear();
}

GLWSelectorEntry *GLWDropDown::getCurrentEntry()
{
	return current_;
}

int GLWDropDown::getCurrentPosition()
{
	std::list<GLWSelectorEntry>::iterator itor;
	int pos = 0;
	for (itor = texts_.begin();
		itor != texts_.end();
		++itor, pos++)
	{
		GLWSelectorEntry &entry = *itor;
		if (current_ == &entry)
		{
			return pos;
		}
	}

	return -1;
}

void GLWDropDown::setCurrentPosition(int pos)
{
	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		++itor)
	{
		GLWSelectorEntry &entry = *itor;
		current_ = &entry;
		if (position++ >= pos) break;
	}

	if (handler_)
	{
		handler_->select(id_, pos, *current_);
	}
}

void GLWDropDown::addEntry(GLWSelectorEntry text)
{
	texts_.push_back(text);
	if (!current_)
	{
		current_ = &texts_.back();
	}
}

void GLWDropDown::draw()
{
	GLWidget::draw();

	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 10.0f, false);
	glEnd();

	button_.draw();
	float offset = 0.0f;
	if(button_.getPressed()) offset = 1.0f;

	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex2d(x_ + w_ - 6.0f + offset, y_ + 17.0f - offset);
		glVertex2d(x_ + w_ - 17.0f + offset, y_ + 17.0f - offset);
		glVertex2d(x_ + w_ - 12.0f + offset, y_ + 7.0f - offset);
	glEnd();
}

void GLWDropDown::buttonDown(unsigned int id)
{
	if (button_.getPressed())
	{
		GLWSelector::instance()->showSelector(
			this, 
			GLWTranslate::getPosX() + x_, 
			GLWTranslate::getPosY() + y_ - 7.0f, 
			texts_,
			0,
			false);
	}
}

void GLWDropDown::buttonUp(unsigned int id)
{
}

void GLWDropDown::mouseDown(int button, float x, float y, bool &skipRest)
{
	button_.mouseDown(button, x, y, skipRest);
	if (!skipRest)
	{
		if (inBox(x, y, x_, y_, w_, h_))
		{
			skipRest = true;
			button_.getPressed() = true;
			buttonDown(0);
		}
	}
}

void GLWDropDown::setX(float x)
{
	GLWidget::setX(x);
	button_.setX(x + w_ - 22.0f);
}

void GLWDropDown::setY(float y)
{
	GLWidget::setY(y);
	button_.setY(y + 2.0f); 
}

void GLWDropDown::itemSelected(GLWSelectorEntry *entry, int pos)
{
	button_.getPressed() = false;

	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		++itor)
	{
		GLWSelectorEntry &entry = *itor;
		current_ = &entry;
		if (position++ >= pos) break;
	}

	if (handler_)
	{
		handler_->select(id_, position, *current_);
	}
}

void GLWDropDown::noItemSelected()
{
	button_.getPressed() = false;
}

void GLWDropDown::mouseUp(int button, float x, float y, bool &skipRest)
{
	button_.mouseUp(button, x, y, skipRest);
}
