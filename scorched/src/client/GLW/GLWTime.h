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

#if !defined(AFX_GLWTime_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_)
#define AFX_GLWTime_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_

#include <string>
#include <GLW/GLWidget.h>
#include <common/Vector.h>

class GLWTime : public GLWidget  
{
public:
	GLWTime(float x = 0.0f, float y = 0.0f, 
		float size = 14.0f);
	virtual ~GLWTime();

	virtual float getW() { calcWidth(); return GLWidget::getW(); }
	virtual void draw();

	void calcWidth();
	virtual bool initFromXML(XMLNode *node);

	REGISTER_CLASS_HEADER(GLWTime);

protected:
	Vector color_;
	float size_;

};

#endif // !defined(AFX_GLWTime_H__75483479_A6F8_45CC_8E83_B517E721211F__INCLUDED_)
