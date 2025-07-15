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

#include <GLW/GLWTabContainer.h>

REGISTER_CLASS_SOURCE(GLWTabContainer);

GLWTabContainer::GLWTabContainer(float x, float y, float w, float h) :
	GLWPanel(x, y, w, h, false, false)
{
}

GLWTabContainer::~GLWTabContainer()
{
}

void GLWTabContainer::layout()
{
	float w = 0.0f;
	float h = 0.0f;

	std::list<GLWPanelEntry>::iterator itor;
	for (itor = widgets_.begin();
		itor != widgets_.end();
		++itor)
	{
		GLWPanelEntry &entry = *itor;

		// Size this widget
		entry.widget->layout();

		w = MAX(entry.widget->getW(), w);
		h = MAX(entry.widget->getH() + 20.0f, h);
	}

	setW(w);
	setH(h);

	for (itor = widgets_.begin();
		itor != widgets_.end();
		++itor)
	{
		GLWPanelEntry &entry = *itor;

		entry.widget->setX(0.0f);
		entry.widget->setY(0.0f);
		entry.widget->setW(w);
		entry.widget->setH(h - 20.0f);
	}
}

void GLWTabContainer::setX(float x)
{
	GLWPanel::setX(x);
}

void GLWTabContainer::setY(float y)
{
	GLWPanel::setY(y);
}
