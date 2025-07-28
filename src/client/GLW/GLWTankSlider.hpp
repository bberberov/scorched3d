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

#ifndef _GLWTankSlider_H_
#define _GLWTankSlider_H_

#include <GLW/GLWDragger.h>

class GLWTankSlider : public GLWDragger, 
	public GLWDraggerI
{
public:
	GLWTankSlider();
	virtual ~GLWTankSlider();

	virtual void draw();
	virtual void currentChanged(unsigned int id, float value);

	REGISTER_CLASS_HEADER(GLWTankSlider);

};

#endif /* _GLWTankSlider_H_ */
