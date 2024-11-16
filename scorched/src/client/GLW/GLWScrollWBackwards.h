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

#if !defined(AFX_GLWScrollWBackwards_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_)
#define AFX_GLWScrollWBackwards_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_

#include <GLW/GLWScrollW.h>

class GLWScrollWBackwards  : 
	public GLWScrollW
{
public:
	GLWScrollWBackwards(float x = 0.0f, float y = 0.0f, 
		float h = 0.0f, 
		int min = 0, int max = 0, int see = 1);
	virtual ~GLWScrollWBackwards();

	virtual void setCurrent(int c);
	virtual int getCurrent();
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	REGISTER_CLASS_HEADER(GLWScrollWBackwards);
protected:

};

#endif // !defined(AFX_GLWScrollWBackwards_H__DCD17624_BF31_45DD_8766_2710AB9FFC6D__INCLUDED_)
