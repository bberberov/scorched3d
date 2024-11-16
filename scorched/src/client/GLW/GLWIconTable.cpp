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

#include <GLW/GLWIconTable.h>
#include <GLEXT/GLState.h>
#include <common/DefinesString.h>

GLWIconTable::GLWIconTable(
	float x, float y, float w, float h,
	std::list<Column> *columns,
	float rowHeight) :
	GLWidget(x, y, w, h),
	rowHeight_(rowHeight),
	scrollBar_(x + w_ - 17.0f, y + 2.0f, h_ - 4.0f, 0, 0, int((h - 25.0f) / rowHeight)),
	selected_(-1), handler_(0), itemCount_(0)
{
	if (columns)
	{
		float colx = x + 1;
		std::list<Column>::iterator itor;
		for (itor = columns->begin();
			itor != columns->end();
			++itor)
		{
			Column &col = *itor;
			GLWTextButton *button = new
				GLWTextButton(
				col.name, 
				colx, y_ + h_ - 20.0f, 
				col.width, this, 
				GLWButton::ButtonFlagCenterX, 10);
			columns_.push_back(button);

			colx += col.width + 2.0f;
		}
	}
}

GLWIconTable::~GLWIconTable()
{
}

void GLWIconTable::setItemCount(int items)
{ 
	if (selected_ > items) selected_ = -1;

	scrollBar_.setMax(items);
	scrollBar_.setCurrent(0);
	itemCount_ = items; 
}

void GLWIconTable::simulate(float frameTime)
{
	scrollBar_.simulate(frameTime);
}

void GLWIconTable::draw()
{
	GLWidget::draw();

	// Draw scroll bar
	scrollBar_.draw();

	// Draw the surround
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 5.0f, false);
	glEnd();

	// Draw all the column headers
	for (int j=0; j<(int) columns_.size(); j++)
	{
		GLWTextButton *column = columns_[j];
		if (!column->getEmpty()) column->draw();
	}

	// Figure out how many rows to show
	int min = MIN(itemCount_, scrollBar_.getCurrent());
	int max = MIN(itemCount_, scrollBar_.getCurrent() + scrollBar_.getSee());

	float y = y_ + h_ - 25.0f;

	// Draw all rows
	for (int i=min; i<max; i++)
	{
		y -= rowHeight_;

		// Draw the divider lines
		if (selected_ != i) 
		{
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x0F0F);
			glColor3f(0.4f, 0.4f, 0.6f);
		}
		else
		{
			glLineWidth(2.0f);
			glColor3f(0.1f, 0.1f, 0.3f);
		}
		glBegin(GL_LINES);
			for (int j=1; j<(int) columns_.size(); j++)
			{
				GLWTextButton *column = columns_[j];
				glVertex2f(column->getX(), y);
				glVertex2f(column->getX(), y + rowHeight_);
			}				
			glVertex2f(x_, y);
			glVertex2f(x_ + w_ - 18.0f, y);
			if (i == min || selected_ == i)
			{
				glVertex2f(x_, y + rowHeight_);
				glVertex2f(x_ + w_ - 18.0f, y + rowHeight_);
			}
		glEnd();
		glDisable(GL_LINE_STIPPLE);
		glLineWidth(1.0f);

		// The user draws the line
		for (int j=0; j<(int) columns_.size(); j++)
		{
			GLWTextButton *column = columns_[j];
			if (handler_) handler_->drawColumn(id_, i, j, 
				column->getX(), y, column->getW());
		}	
	}
}

void GLWIconTable::buttonDown(unsigned int id)
{
	for (int j=0; j<(int) columns_.size(); j++)
	{
		GLWTextButton *column = columns_[j];
		if (column->getId() == id && handler_)
		{
			handler_->columnSelected(id_, j);
		}
	}
}

void GLWIconTable::mouseDown(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDown(button, x, y, skipRest);
	if (!skipRest)
	{
		if (inBox(x, y, x_, y_, w_ - 20.0f, h_ - 20.0f))
		{
			skipRest = true;

			int pos = int((y_ + h_ - 25.0f - y) / rowHeight_) + scrollBar_.getCurrent();
			if (pos >=0 && pos < itemCount_)
			{
				selected_ = pos;

				if (handler_) handler_->rowSelected(getId(), selected_);

				if (button == GameState::MouseButtonLeftDoubleClick)
				{
					if (handler_) handler_->rowChosen(getId(), selected_);
				}
			}
		}
	}

	if (!skipRest)
	{
		std::vector<GLWTextButton *>::iterator itor;
		for (itor = columns_.begin();
			itor != columns_.end();
			++itor)
		{
			GLWTextButton *column = (*itor);
			column->mouseDown(button, x, y, skipRest);
		}
	}
}

void GLWIconTable::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDrag(button, mx, my, x, y, skipRest);

	if (!skipRest)
	{
		std::vector<GLWTextButton *>::iterator itor;
		for (itor = columns_.begin();
			itor != columns_.end();
			++itor)
		{
			GLWTextButton *column = (*itor);
			column->mouseDrag(button, mx, my, x, y, skipRest);
		}
	}
}

void GLWIconTable::mouseUp(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseUp(button, x, y, skipRest);

	if (!skipRest)
	{
		std::vector<GLWTextButton *>::iterator itor;
		for (itor = columns_.begin();
			itor != columns_.end();
			++itor)
		{
			GLWTextButton *column = (*itor);
			column->mouseUp(button, x, y, skipRest);
		}
	}
}

void GLWIconTable::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		scrollBar_.mouseWheel(scrollBar_.getX() + 1, scrollBar_.getY() + 1, z, skipRest);
	}
}
