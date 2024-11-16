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

#include <GLW/GLWTime.h>
#include <GLW/GLWFont.h>
#include <XML/XMLNode.h>

REGISTER_CLASS_SOURCE(GLWTime);

GLWTime::GLWTime(float x, float y, float size) : 
	GLWidget(x, y, 0.0f, 20.0f), size_(size),
	color_(GLWFont::widgetFontColor)
{
	tooltipTransparent_ = true;
}

GLWTime::~GLWTime()
{

}

void GLWTime::calcWidth()
{
	time_t t = time(0);
	const char *timeStr = ctime(&t);
	if (w_ == 0.0f) w_ = GLWFont::instance()->getGameFont()->getWidth(
		size_, (char *) timeStr);
}

bool GLWTime::initFromXML(XMLNode *node)
{
	node->getNamedChild("fontsize", size_, false);
	node->getNamedChild("fontcolor", color_, false);

	return GLWidget::initFromXML(node);
}

void GLWTime::draw()
{
	GLWidget::draw();

	time_t t = time(0);
	char *timeStr = ctime(&t);

	glColor3f(1.0f, 0.0f, 0.0f);
	calcWidth();
	GLWFont::instance()->getGameFont()->drawSubStr(
		0, (int) strlen(timeStr) - 1,
		color_, size_,
		x_, y_ + 6.0f, 0.0f, timeStr);
}
