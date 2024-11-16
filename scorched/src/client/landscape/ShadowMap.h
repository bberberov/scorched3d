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

#if !defined(AFX_SHADOWMAP_H__50A1C655_51B8_4B47_982E_58C20CD21815__INCLUDED_)
#define AFX_SHADOWMAP_H__50A1C655_51B8_4B47_982E_58C20CD21815__INCLUDED_

#include <GLEXT/GLTexture.h>

class ShadowMap  
{
public:
	ShadowMap();
	virtual ~ShadowMap();

	void setTexture();
	void addSquare(float x, float y, float w, float opacity = 1.0f);
	void addCircle(float sx, float sy, float sw, float opacity = 1.0f);

	bool shouldAddShadow();

	unsigned int getShadowCount() { return shadowCount_; }

protected:
	unsigned int shadowCount_;
	GLTexture shadowTexture_;
	GLubyte *shadowBytes_;
	GLint size_, sizeSq_;

	void addShadow(float sx, float sy, float sw, float opacity, bool circle);

};

#endif // !defined(AFX_SHADOWMAP_H__50A1C655_51B8_4B47_982E_58C20CD21815__INCLUDED_)
