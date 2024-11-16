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

#ifndef __DEFINESASSERT__
#define __DEFINESASSERT__

#include <string>

namespace S3D
{
	void dialogAssert(const char *lineText, const int line, const char *file);
	void glAssert(unsigned int e, const int line, const char *file);

	void dialogMessage(const std::string &header, const std::string &text, bool split = true);
	void dialogExit(const std::string &header, const std::string &text, bool split = true);
}

#ifndef S3D_SERVER
//#define GL_ASSERT() { GLenum e=glGetError(); if(e!= GL_NO_ERROR) S3D::glAssert(e, __LINE__, __FILE__); }
#define GL_ASSERT() {}
#else
#define GL_ASSERT() {}
#endif

#define DIALOG_ASSERT(x) if(!(x)) S3D::dialogAssert(#x, __LINE__, __FILE__);

#endif // __DEFINESASSERT__
