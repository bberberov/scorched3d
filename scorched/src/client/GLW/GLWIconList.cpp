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

#include <GLW/GLWIconList.h>
#include <GLEXT/GLState.h>
#include <common/DefinesString.h>

GLWIconList::GLWIconList(
	float x, float y, float w, float h,
	float squaresHeight,
	unsigned int flags) :
	GLWidget(x, y, w, h),
	squaresHeight_(squaresHeight),
	scrollBar_(x + w_ - 17.0f, y + 2.0f, h_ - 4.0f, 0, 0, int(h / squaresHeight)),
	selected_(-1), handler_(0), flags_(flags)
{
}

GLWIconList::~GLWIconList()
{
}

void GLWIconList::addItem(GLWIconListItem *item)
{
	if (selected_ == -1)
	{
		selected_ = 0;
		if (handler_) handler_->selected(getId(), selected_);
	}

	items_.push_back(item);
	scrollBar_.setMax((int) items_.size());
	scrollBar_.setCurrent(0);
}

void GLWIconList::clear()
{
	selected_ = -1;
	while (!items_.empty())
	{
		GLWIconListItem *item = items_.back();
		items_.pop_back();
		delete item;
	}
}

GLWIconListItem *GLWIconList::getSelected()
{
	if (selected_ < 0 || selected_ >= (int) items_.size()) return 0;
	return items_[selected_];
}

void GLWIconList::simulate(float frameTime)
{
	scrollBar_.simulate(frameTime);
}

void GLWIconList::draw()
{
	GLWidget::draw();
	scrollBar_.draw();

	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 5.0f, false);
	glEnd();

	int min = MIN((int) items_.size(), scrollBar_.getCurrent());
	int max = MIN((int) items_.size(), scrollBar_.getCurrent() + scrollBar_.getSee());

	float x = x_ + 5.0f; 
	float y = y_ + h_;
	for (int i=min; i<max; i++)
	{
		y -= squaresHeight_;

		if (!(flags_ & eNoDrawSelected) && selected_ == i)
		{
			float SelectW = w_ - 27.0f;
			float SelectH = squaresHeight_ - 5.0f;
			glColor3f(0.4f, 0.4f, 0.6f);
			glBegin(GL_LINE_LOOP);
				glVertex2f(x, y);
				glVertex2f(x + SelectW, y);
				glVertex2f(x + SelectW, y + SelectH);
				glVertex2f(x, y + SelectH);
			glEnd();
		}

		GLWIconListItem *item = items_[i];
		item->draw(x, y, w_ - 30.0f);
	}
}

void GLWIconList::mouseDown(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDown(button, x, y, skipRest);
	if (!skipRest)
	{
		if (inBox(x, y, x_, y_, w_ - 20.0f, h_))
		{
			skipRest = true;

			int pos = int((y_ + h_ - y) / squaresHeight_) + scrollBar_.getCurrent();
			if (pos >=0 && pos < (int) items_.size())
			{
				selected_ = pos;

				if (handler_) handler_->selected(getId(), selected_);

				if (button == GameState::MouseButtonLeftDoubleClick)
				{
					if (handler_) handler_->chosen(getId(), selected_);
				}
			}
		}
	}
}

void GLWIconList::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWIconList::mouseUp(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseUp(button, x, y, skipRest);
}

void GLWIconList::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		scrollBar_.mouseWheel(scrollBar_.getX() + 1, scrollBar_.getY() + 1, z, skipRest);
	}
}
