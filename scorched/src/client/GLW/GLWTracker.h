////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#ifndef _GLWTRACKER_H_
#define _GLWTRACKER_H_

#include <GLW/GLWidget.h>

class GLWTrackerI
{
public:
	virtual void currentChanged(unsigned int id, float valueX, float valueY) = 0;
};

class GLWTracker : public GLWidget
{
public:
	GLWTracker(float x = 0.0f, float y = 0.0f, float w = 0.0f, float range = 0.0f);
	virtual ~GLWTracker();

	void setHandler(GLWTrackerI *handler) { handler_ = handler; }

	float getCurrentX() { return currentX_; }
	float getCurrentY() { return currentY_; }
	void setCurrentX(float currentx) { currentX_ = currentx; }
	void setCurrentY(float currenty) { currentY_ = currenty; }

	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);

	REGISTER_CLASS_HEADER(GLWTracker);

protected:
	GLWTrackerI *handler_;
	bool dragging_;
	float currentX_, currentY_;
	float range_;

};

class GLWTankTracker : public GLWTracker,
	public GLWTrackerI
{
public:
	GLWTankTracker();
	virtual ~GLWTankTracker();

	virtual void draw();
	virtual void currentChanged(unsigned int id, float valueX, float valueY);

	REGISTER_CLASS_HEADER(GLWTankTracker);

};

#endif /* _GLWTRACKER_H_ */
