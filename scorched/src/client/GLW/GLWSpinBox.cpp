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

#include <common/Keyboard.h>
#include <common/DefinesString.h>
#include <GLW/GLWSpinBox.h>
#include <GLW/GLWFont.h>

REGISTER_CLASS_SOURCE(GLWSpinBox);

GLWSpinBox::GLWSpinBox(float x, float y, float w, int start, 
					   int minRange, int maxRange, int step) :
	GLWidget(x, y, w, 25.0f), value_(start), minRange_(minRange), maxRange_(maxRange),
	top_(x + w - 12.0f, y + 13.0f, 10.0f, 10.0f),
	bottom_(x + w - 12.0f, y + 2.0f, 10.0f, 10.0f),
	dragging_(false), step_(step)
{
	top_.setHandler(this);
	bottom_.setHandler(this);
}

GLWSpinBox::~GLWSpinBox()
{

}

void GLWSpinBox::draw()
{
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 10.0f, false);
	glEnd();

	top_.draw();
	bottom_.draw();

	float topOffset = 0.0f;
	if(top_.getPressed()) topOffset = 1.0f;

	float botOffset = 0.0f;
	if(bottom_.getPressed()) botOffset = 1.0f;

	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex2d(x_ + w_ - 11.0f + topOffset, y_ + 15.0f - topOffset);
		glVertex2d(x_ + w_ - 4.0f + topOffset, y_ + 15.0f - topOffset);
		glVertex2d(x_ + w_ - 7.0f + topOffset, y_ + 21.0f - topOffset);

		glVertex2d(x_ + w_ - 4.0f + botOffset, y_ + 9.0f - botOffset);
		glVertex2d(x_ + w_ - 10.0f + botOffset, y_ + 9.0f - botOffset);
		glVertex2d(x_ + w_ - 7.0f + botOffset, y_ + 4.0f - botOffset);
	glEnd();

	GLWFont::instance()->getGameFont()->draw(
		GLWFont::widgetFontColor, 14,
		x_ + 5.0f, y_ + 5.0f, 0.0f, 
		S3D::formatStringBuffer("%i", value_));
}

void GLWSpinBox::mouseDown(int button, float x, float y, bool &skipRest)
{
	top_.mouseDown(button, x, y, skipRest);
	if (skipRest) return;

	bottom_.mouseDown(button, x, y, skipRest);
	if (!skipRest)
	{
		if (inBox(x, y, x_, y_, w_, h_))
		{
			dragging_ = true;
			skipRest = true;
		}
	}
}

void GLWSpinBox::mouseUp(int button, float x, float y, bool &skipRest)
{
	dragging_ = false;

	top_.mouseUp(button, x, y, skipRest);
	if (skipRest) return;

	bottom_.mouseUp(button, x, y, skipRest);
}

void GLWSpinBox::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		if (y < 0) if (value_ > minRange_) value_ -= step_;
		if (y > 0) if (value_ < maxRange_) value_ += step_;
		skipRest = true;
		return;
	}

	top_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;

	bottom_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;
}

void GLWSpinBox::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	for (int i=0; i<hisCount; i++)
	{
		unsigned int dik = history[i].sdlKey;
		switch (dik)
		{
			case SDLK_UP:
			case SDLK_EQUALS:
				if (value_ < maxRange_) value_ += step_;
				skipRest = true;
				break;
			case SDLK_DOWN:
			case SDLK_MINUS:
				if (value_ > minRange_) value_ -= step_;
				skipRest = true;
				break;
		}
	}
}

void GLWSpinBox::buttonDown(unsigned int id)
{
	if (id == top_.getId())
	{
		if (value_ < maxRange_) value_ += step_;
	}
	else if (id == bottom_.getId())
	{
		if (value_ > minRange_) value_ -= step_;
	}
}
