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

#include <GLW/GLWFileView.h>
#include <GLW/GLWFont.h>
#include <common/DefinesString.h>

REGISTER_CLASS_SOURCE(GLWFileView);

GLWFileView::GLWFileView(char *fileName, float x, float y, float w, float h) :
	GLWidget(x, y, w, h), 
	scroll_(x + w - 17, y, h, 0, 1)
{
	if (fileName[0])
	{
		lines_.readFile(fileName);
		scroll_.setMax((int) lines_.getLines().size());
		scroll_.setSee((int) (h_ / 9));
		scroll_.setCurrent(scroll_.getMax());
	}
}

GLWFileView::~GLWFileView()
{
}

void GLWFileView::simulate(float frameTime)
{
	scroll_.simulate(frameTime);
}

void GLWFileView::draw()
{
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 6.0f, false);
	glEnd();

	float posY = y_ + h_ - 10.0f;
	for (int i=scroll_.getMax() - scroll_.getCurrent(); i<(int) lines_.getLines().size(); i++)
	{
		GLWFont::instance()->getGameFont()->drawWidth(
			w_,
			GLWFont::widgetFontColor, 8,
			x_ + 5.0f, posY, 0.0f, 
			S3D::formatStringBuffer("%s", lines_.getLines()[i].c_str()));
		posY -= 9.0f;

		if (posY < y_) break;
	}

	scroll_.draw();
}

void GLWFileView::mouseDown(int button, float x, float y, bool &skipRest)
{
	scroll_.mouseDown(button, x, y, skipRest);
}

void GLWFileView::mouseUp(int button, float x, float y, bool &skipRest)
{
	scroll_.mouseUp(button, x, y, skipRest);
}

void GLWFileView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	scroll_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWFileView::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (z < 0.0f) scroll_.setCurrent(scroll_.getCurrent() + 1);
		else scroll_.setCurrent(scroll_.getCurrent() - 1);
	}
}
