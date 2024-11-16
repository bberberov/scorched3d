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

#if !defined(AFX_GLWFLAG_H__E5E95ACF_A6F4_4552_B05C_75CA02ACB6D2__INCLUDED_)
#define AFX_GLWFLAG_H__E5E95ACF_A6F4_4552_B05C_75CA02ACB6D2__INCLUDED_

#include <GLW/GLWFont.h>
#include <GLW/GLWidget.h>
#include <common/Vector.h>

class GLWFlag : public GLWidget
{
public:
	GLWFlag(Vector &color = GLWFont::widgetFontColor, 
		float x = 0.0f, float y = 0.0f, float w = 0.0f);
	virtual ~GLWFlag();

	virtual void simulate(float frameTime);
	virtual void draw();

	float getOffset() { return offset_; }
	void setOffset(float offset) { offset_ = offset; }
	void setColor(Vector &color) { color_ = color; }
	Vector &getColor() { return color_; }

	REGISTER_CLASS_HEADER(GLWFlag);
protected:
	Vector color_;
	float offset_;

};

#endif // !defined(AFX_GLWFLAG_H__E5E95ACF_A6F4_4552_B05C_75CA02ACB6D2__INCLUDED_)
