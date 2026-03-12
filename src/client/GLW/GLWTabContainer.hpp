////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_GLWTabContainer_hpp_INCLUDE__
#define __INCLUDE_GLWTabContainer_hpp_INCLUDE__

#include <GLW/GLWPanel.hpp>
#include <GLW/GLWLabel.hpp>

class GLWTabContainer : public GLWPanel
{
public:
	GLWTabContainer( float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f );
	virtual ~GLWTabContainer();

	virtual void layout();

	virtual void setX( float x );
	virtual void setY( float y );

	REGISTER_CLASS_HEADER( GLWTabContainer );
};

#endif  // __INCLUDE_GLWTabContainer_hpp_INCLUDE__
