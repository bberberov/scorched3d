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

#ifndef _GLWSPINBOX_H_
#define _GLWSPINBOX_H_

#include <GLW/GLWButton.h>

class GLWSpinBox : public GLWidget, 
					public GLWButtonI
{
public:
	GLWSpinBox(float x = 0.0f, float y = 0.0f, float w = 0.0f, 
		int start = 0, int minRange = 0, int maxRange = 0, int step = 1);
	virtual ~GLWSpinBox();

	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

	virtual void buttonDown(unsigned int id);
	int getValue() { return value_; }

	REGISTER_CLASS_HEADER(GLWSpinBox);

protected:
	bool keyDown_;
	bool dragging_;
	int step_;
	int value_;
	int minRange_, maxRange_;
	GLWButton top_, bottom_;

};

#endif /* _GLWSPINBOX_H_ */
