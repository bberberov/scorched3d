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

#include <GLEXT/GLState.h>
#include <GLW/GLWTab.h>

static const float tabRoundSize = 10.0f;
static const float tabHeight = 20.0f;
static const float tabSpacing = 10.0f;

REGISTER_CLASS_SOURCE(GLWTab);

GLWTabI::~GLWTabI()
{

}

GLWTab::GLWTab(const std::string &tabName, 
	const LangString &tabLabel,
	float x, float y, float w, float h) :
	GLWScrollPanel(x, y, w, h), index_(-1.0f),
	label_(x + 5.0f + -1.0f, y + h - 3.0f, tabLabel),
	name_(tabName),
	handler_(0), depressed_(true)
{
	label_.setSize(12.0f);
}

GLWTab::~GLWTab()
{
}

void GLWTab::setH(float h)
{
	GLWScrollPanel::setH(h);
	label_.setY(y_ + h_ - 3.0f);
}

float GLWTab::getTw()
{
	return label_.getW() + tabSpacing;
}

void GLWTab::mouseDown(int button, float x, float y, bool &skipRest)
{
	float tw = getTw();
	if (x > x_ + index_ &&
		x < x_ + index_ + tw &&
		y < y_ + h_ + tabHeight &&
		y > y_ + h_)
	{
		setDepressed();
		skipRest = true;
	}

	if (!skipRest && depressed_)
	{
		GLWScrollPanel::mouseDown(button, x, y, skipRest);
	}
}

void GLWTab::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (!skipRest && depressed_)
	{
		GLWScrollPanel::mouseWheel(x, y, z, skipRest);
	}
}

void GLWTab::setDepressed()
{
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = parent_->getWidgets().begin();
		itor != parent_->getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			tab->depressed_ = false;
		}
	}

	depressed_ = true;
	if (handler_) handler_->tabDown(getId());
}

void GLWTab::setParent(GLWPanel *parent)
{
	GLWScrollPanel::setParent(parent);

	setDepressed();
}

void GLWTab::draw()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);

	if (index_ == -1.0f)
	{
		index_ = 0.0f;
		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = parent_->getWidgets().begin();
			itor != parent_->getWidgets().end();
			++itor)
		{
			GLWPanel::GLWPanelEntry &entry = (*itor);
			if (entry.widget->getMetaClassId() == getMetaClassId())
			{
				GLWTab *tab = (GLWTab *) entry.widget;
				if (tab == this) break;
				index_ += tab->getTw() + 2.0f;
			}
		}
	}

	label_.setX(x_ + 5.0f + index_);
	if (depressed_) drawSurround();
	else drawNonSurround();
}

void GLWTab::drawNonSurround()
{
	float tw = getTw();
	glBegin(GL_LINE_STRIP);
		glColor3f(0.4f, 0.4f, 0.6f);
		glVertex2f(x_ + index_, y_ + h_);
		drawCircle(12, 16, x_ + index_ + tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);
		drawCircle(0, 2, x_ + index_ + tw - tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCircle(2, 4, x_ + index_ + tw - tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);
		glVertex2f(x_ + index_ + tw, y_ + h_);
	glEnd();

	label_.draw();
}

void GLWTab::drawSurround()
{
	float tw = getTw();

	glBegin(GL_LINE_STRIP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(x_, y_);
		glVertex2f(x_, y_ + h_);
		glVertex2f(x_ + index_, y_ + h_);
		drawCircle(12, 16, x_ + index_ + tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);
		drawCircle(0, 2, x_ + index_ + tw - tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);

		glColor3f(0.4f, 0.4f, 0.6f);
		drawCircle(2, 4, x_ + index_ + tw - tabRoundSize, 
			y_ + h_ + tabHeight - tabRoundSize, tabRoundSize);
		glVertex2f(x_ + index_ + tw, y_ + h_);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(x_ + index_ + tw, y_ + h_);
		glVertex2f(x_ + w_, y_ + h_);

		glColor3f(0.4f, 0.4f, 0.6f);
		glVertex2f(x_ + w_, y_ + h_);
		glVertex2f(x_ + w_, y_);
		glVertex2f(x_, y_);
	glEnd();

	GLWScrollPanel::draw();

	label_.draw();
}
