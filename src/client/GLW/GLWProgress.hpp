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


// GLWProgress.h: interface for the GLWProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLWPROGRESS_H__0712973C_EA7A_4C72_A48B_90B2A898FAF3__INCLUDED_)
#define AFX_GLWPROGRESS_H__0712973C_EA7A_4C72_A48B_90B2A898FAF3__INCLUDED_


#include <GLW/GLWPanel.h>

class GLWProgress : public GLWPanel
{
public:
	GLWProgress(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float min = 0.0f, float max = 100.0f);
	virtual ~GLWProgress();

	virtual void draw();

	const float getCurrent() { return current_; }
	void setCurrent(float newCurrent);

	REGISTER_CLASS_HEADER(GLWProgress);

protected:
	float x_, y_, w_;
	float min_, max_;
	float current_;

};

#endif // !defined(AFX_GLWPROGRESS_H__0712973C_EA7A_4C72_A48B_90B2A898FAF3__INCLUDED_)
