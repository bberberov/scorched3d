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

#ifndef __INCLUDE_GLInfo_hpp_INCLUDE__
#define __INCLUDE_GLInfo_hpp_INCLUDE__

class GLInfo
{
public:
	// clang-format off
	// uncrustify off
	static unsigned int getNoTriangles()                     { return noTriangles_; }
	static void         addNoTriangles( unsigned int count ) { noTriangles_ += count; }
	static void         resetNoTriangles()                   { noTriangles_ = 0; }
	// uncrustify on
	// clang-format on

protected:
	static unsigned int noTriangles_;

private:
	GLInfo();
	virtual ~GLInfo();
};

#endif  // __INCLUDE_GLInfo_hpp_INCLUDE__
