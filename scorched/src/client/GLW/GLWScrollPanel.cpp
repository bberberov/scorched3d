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

#include <GLW/GLWScrollPanel.h>
#include <GLW/GLWTranslate.h>
#include <GLEXT/GLState.h>
#include <float.h>

REGISTER_CLASS_SOURCE(GLWScrollPanel);

GLWScrollPanel::GLWScrollPanel(float x, float y, float w, float h) : 
	GLWPanel(x, y, w, h), scrollW_(x + w - 19, y + 4, h - 10, 0, 0),
	drawScrollBar_(true), maxSee_(0), widgetHeight_(0.0f)
{
	scrollW_.setHandler(this);
}

GLWScrollPanel::~GLWScrollPanel()
{

}

void GLWScrollPanel::setH(float h)
{
	GLWidget::setH(h);

	scrollW_.setH(h_ - 10);
}

void GLWScrollPanel::simulate(float frameTime)
{
	scrollW_.simulate(frameTime);
}

void GLWScrollPanel::calculateVisible()
{
	drawScrollBar_ = false;
	int canSee = 0;
	float widgetMinY = FLT_MAX;
	float widgetMaxY = FLT_MIN;
	{
		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
				GLWidget *vw = (GLWidget *) (*itor).widget;
				if (vw->getY() < 0.0f || vw->getY() + vw->getH() > h_)
				{
					drawScrollBar_ = true;
				}
				else
				{
					canSee++;
				}

				if (vw->getY() < widgetMinY) widgetMinY = vw->getY();
				if (vw->getY() + vw->getH() > widgetMaxY) widgetMaxY = vw->getY() + vw->getH();
		}
	}

	widgetHeight_ = widgetMaxY - widgetMinY;

	// Draw scroll bar if not
	if (drawScrollBar_)
	{
		if (canSee > maxSee_) maxSee_ = canSee;
		scrollW_.setSee(maxSee_);
		scrollW_.setMax((int) getWidgets().size());
	}
}

void GLWScrollPanel::draw()
{
	calculateVisible();

	glPushMatrix();
	{
		GLWTranslate trans(x_, y_);
		glTranslatef(x_, y_, 0.0f);

		GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);

		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
			glPushMatrix();
				GLWidget *vw = (GLWidget *) (*itor).widget;
				if (vw->getY() >= 0.0f && vw->getY() + vw->getH() <= h_)
				{
					vw->draw();
				}
			glPopMatrix();
		}
	}
	glPopMatrix();

	if (drawScrollBar_) scrollW_.draw();
}

void GLWScrollPanel::mouseUp(int button, float x, float y, bool &skipRest)
{
	if (drawScrollBar_)
	{
		scrollW_.mouseUp(button, x, y, skipRest);
	}
	if (!skipRest)
	{
		x -= x_;
		y -= y_;

		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
			GLWidget *vw =
				(GLWidget *) (*itor).widget;
			if (vw->getY() < 0.0f || vw->getY() + vw->getH() > h_)
			{
			
			}
			else
			{
				vw->mouseUp(button, x, y, skipRest);
			}
			if (skipRest) break;
		}
	}
}

void GLWScrollPanel::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (drawScrollBar_)
	{
		scrollW_.mouseDown(button, x, y, skipRest);
	}
	if (!skipRest)
	{
		x -= x_;
		y -= y_;

		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
			GLWidget *vw =
				(GLWidget *) (*itor).widget;
			if (vw->getY() < 0.0f || vw->getY() + vw->getH() > h_)
			{
			
			}
			else
			{
				vw->mouseDown(button, x, y, skipRest);
			}
			if (skipRest) break;
		}
	}
}

void GLWScrollPanel::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (drawScrollBar_)
	{
		scrollW_.mouseDrag(button, mx, my, x, y, skipRest);
	}
	if (!skipRest)
	{
		mx -= x_;
		my -= y_;

		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
			GLWidget *vw =
				(GLWidget *) (*itor).widget;
			if (vw->getY() < 0.0f || vw->getY() + vw->getH() > h_)
			{
			
			}
			else
			{
				vw->mouseDrag(button, mx, my, x, y, skipRest);
			}
			if (skipRest) break;
		}
	}
}

void GLWScrollPanel::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (drawScrollBar_)
		{
			if (z < 0.0f) scrollW_.setCurrent(scrollW_.getCurrent() + 2);
			else scrollW_.setCurrent(scrollW_.getCurrent() - 2);
		}
		else 
		{
			GLWPanel::mouseWheel(x, y, z, skipRest);
		}
	}
}

void GLWScrollPanel::positionChange(unsigned int id, int current, int movement)
{
	float move = ((widgetHeight_ + 2.0f) / float(getWidgets().size())) * float(movement);

	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWidget *vw =
			(GLWidget *) (*itor).widget;
		vw->setY(vw->getY() - move);
	}
}

void GLWScrollPanel::clear()
{
	scrollW_.setCurrent(0);
	GLWPanel::clear();
}
