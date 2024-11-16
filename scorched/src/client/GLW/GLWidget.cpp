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

#include <GLW/GLWidget.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWToolTip.h>
#include <GLW/GLWTranslate.h>
#include <GLEXT/GLState.h>
#include <XML/XMLParser.h>
#include <common/Defines.h>
#include <math.h>

unsigned int GLWidget::nextId_ = 0;
static GLuint listNo = 0;

GLWidget::GLWidget(float x, float y, float w, float h) : 
	id_(++nextId_), parent_(0), userData_(0), visible_(true),
	x_(x), y_(y), w_(w), h_(h), tooltip_(0), tooltipTransparent_(false)
{
}

GLWidget::~GLWidget()
{
}

void GLWidget::draw()
{
	if (listNo == 0)
	{
		listNo = glGenLists(1);
		glNewList(listNo, GL_COMPILE);
			for (float a=360.0f; a>0.0f; a-=360.0f / 36.0f)
			{
				glVertex2f(sinf(a/180.0f * PI), 
					cosf(a/180.0f * PI));
			}
		glEndList();
	}

	if (tooltipTransparent_)
	{
	}
	else if (tooltip_)
	{
		GLWToolTip::instance()->addToolTip(tooltip_, 
			GLWTranslate::getPosX() + x_, 
			GLWTranslate::getPosY() + y_, 
			w_, h_);
	}
	else
	{
		GLWToolTip::instance()->clearToolTip(
			GLWTranslate::getPosX() + x_, 
			GLWTranslate::getPosY() + y_, 
			w_, h_);
	}
}

void GLWidget::layout()
{

}

void GLWidget::simulate(float frameTime)
{

}

void GLWidget::setParent(GLWPanel *parent)
{
	parent_ = parent;
}

void GLWidget::mouseDown(int button, float x, float y, bool &skipRest)
{

}

void GLWidget::mouseUp(int button, float x, float y, bool &skipRest)
{

}

void GLWidget::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{

}

void GLWidget::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{

}

void GLWidget::mouseWheel(float x, float y, float z, bool &skipRest)
{

}

void GLWidget::display()
{

}

void GLWidget::hide()
{

}

bool GLWidget::initFromXML(XMLNode *node)
{
	node->getNamedChild("name", name_, false);
	node->getNamedChild("visible", visible_, false);
	node->getNamedChild("tooltiptransparent", tooltipTransparent_, false);

	float x, y, w, h;
	if (!node->getNamedChild("x", x)) return false;
	if (!node->getNamedChild("y", y)) return false;
	if (!node->getNamedChild("w", w)) return false;
	if (!node->getNamedChild("h", h)) return false; 
	setX(x);
	setY(y);
	setW(w);
	setH(h);

	return true;
}

void GLWidget::saveSettings(XMLNode *node)
{
}

void GLWidget::loadSettings(XMLNode *node, bool resetPositions)
{
}

void GLWidget::drawWholeCircle(bool cap)
{
	glCallList(listNo);
	if (cap) glVertex2f(0.0f, 1.0f);
}

void GLWidget::drawCircle(int startA, int endA, float posX, float posY, float size)
{
	static Vector positions[16];
	static bool init = false;
	if (!init)
	{
		init = true;
		for (int i=0; i<16; i++)
		{
			const float angDeg = 22.5f;
			float ang = i * angDeg;
			positions[i][0] = sinf(ang / 180.0f * PI);
			positions[i][1] = cosf(ang / 180.0f * PI);
		}
	}

	if (startA < endA)
	{
		for (int b=startA; b<=endA; b++)
		{
			int a=b; if (a>15) a=a-16;
			glVertex2f(posX + positions[a][0] * size, posY + positions[a][1] * size);
		}
	}
	else
	{
		for (int b=startA; b>=endA; b--)
		{
			int a=b; if (a<0) a=16+a;
			glVertex2f(posX + positions[a][0] * size, posY + positions[a][1] * size);
		}
	}
}

void GLWidget::drawRoundBox(float x, float y, float w, float h, float size)
{
	drawCircle(8, 4, x + w - size, y + size, size);
	drawCircle(4, 0, x + w - size, y + h - size, size);
	drawCircle(0, -4, x + size, y + h - size, size);
	drawCircle(-4, -8, x + size, y + size, size);
}

void GLWidget::drawShadedRoundBox(float x, float y, float w, float h, float size, bool depressed)
{
	if (depressed) glColor3f(0.4f, 0.4f, 0.6f);
	else glColor3f(1.0f, 1.0f, 1.0f);

	drawCircle(-6, -8, x + size, y + size, size);
	drawCircle(8, 4, x + w - size, y + size, size);
	drawCircle(4, 3, x + w - size, y + h - size, size);

	if (depressed) glColor3f(1.0f, 1.0f, 1.0f); 
	else glColor3f(0.4f, 0.4f, 0.6f);

	drawCircle(2, 0, x + w - size, y + h - size, size);
	drawCircle(0, -4, x + size, y + h - size, size);
	drawCircle(-4, -5, x + size, y + size, size);

	if (depressed) glColor3f(0.4f, 0.4f, 0.6f);
	else glColor3f(1.0f, 1.0f, 1.0f);
	drawCircle(-6, -6, x + size, y + size, size);
}

void GLWidget::drawBox(float x, float y, float w, float h, bool depressed)
{
	if (depressed) glColor3f(0.4f, 0.4f, 0.6f);
	else glColor3f(1.0f, 1.0f, 1.0f);

	glVertex2f(x , y);
	glVertex2f(x + w, y);

	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);

	if (depressed) glColor3f(1.0f, 1.0f, 1.0f);
	else glColor3f(0.4f, 0.4f, 0.6f);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);

	glVertex2f(x, y + h);
	glVertex2f(x, y);
}

bool GLWidget::inBox(float posX, float posY, float x, float y, float w, float h)
{
	if ((posX >= x) && (posX <= x + w) &&
		(posY >= y) && (posY <= y + h))
	{
		return true;
	}

	return false;
}

GLWCondition::GLWCondition()
{
}

GLWCondition::~GLWCondition()
{
}

bool GLWCondition::initFromXML(XMLNode *node)
{
	return true;
}

