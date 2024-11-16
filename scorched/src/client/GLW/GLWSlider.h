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

#ifndef _GLWSLIDER_H_
#define _GLWSLIDER_H_

#include <GLW/GLWidget.h>

class GLWSliderI
{
public:
	virtual void currentChanged(unsigned int id, float value) = 0;
};

class GLWSlider : public GLWidget
{
public:
	GLWSlider(float x = 0.0f, float y = 0.0f, float w = 0.0f, 
		float min = 0.0f, float max = 100.0f, int marks = 10);
	virtual ~GLWSlider();

	void setHandler(GLWSliderI *handler) { handler_ = handler; }

	float getCurrent() { return current_; }
	void setCurrent(float current);

	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);

	REGISTER_CLASS_HEADER(GLWSlider);

protected:
	GLWSliderI *handler_;
	float current_;
	float min_, max_;
	int marks_;

};

#endif /* _GLWSLIDER_H_ */
