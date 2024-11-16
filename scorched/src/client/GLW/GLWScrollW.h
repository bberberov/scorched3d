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

#if !defined(AFX_GLWSCROLLW_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_)
#define AFX_GLWSCROLLW_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_

#include <GLW/GLWScrollButton.h>

class GLWScrollWI
{
public:
	virtual ~GLWScrollWI();

	virtual void positionChange(unsigned int id, int current, int movement) = 0;
};

class GLWScrollW  : 
	public GLWidget, 
	public GLWButtonI,
	public GLWScrollButtonI
{
public:
	GLWScrollW(float x = 0.0f, float y = 0.0f, 
		float h = 0.0f, 
		int min = 0, int max = 0, int see = 1, 
		GLWScrollWI *handler = 0);
	virtual ~GLWScrollW();

	virtual void setHandler(GLWScrollWI *handler = 0) { handler_ = handler; }
	void setMin(int min) { min_ = min; }
	void setMax(int max) { max_ = max; }
	void setSee(int see) { see_ = see; }
	virtual void setCurrent(int c);

	virtual void setX(float x);
	virtual void setY(float y);
	virtual void setW(float w);
	virtual void setH(float h);

	virtual int getCurrent() { return current_; }
	int getMin() { return min_; }
	int getMax() { return max_; }
	int getSee() { return see_; }

	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void buttonDown(unsigned int id);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	virtual void startDrag(unsigned int id);
	virtual void buttonDrag(unsigned int id, float x, float y);

	REGISTER_CLASS_HEADER(GLWScrollW);
protected:
	GLWScrollWI *handler_;
	GLWButton topButton_;
	GLWButton bottomButton_;
	GLWButton backButtonTop_; // Hidden
	GLWButton backButtonBot_; // Hidden
	GLWScrollButton middleButton_;
	int min_, max_, see_;
	int current_;
	int dragCurrent_;

};


#endif // !defined(AFX_GLWSCROLLW_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_)
