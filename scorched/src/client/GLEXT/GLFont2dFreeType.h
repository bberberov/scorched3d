////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2024
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

#ifndef _GLFONT2DFREETYPE_H_
#define _GLFONT2DFREETYPE_H_

#include <string>
#include <GLEXT/GLFont2dStorage.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

class GLFont2dFreeType
{
public:
	GLFont2dFreeType();
	~GLFont2dFreeType();

	bool createFont(const std::string &typeFace, unsigned int h, bool makeShadow);
	bool createCharacter(unsigned int ch, GLFont2dStorage::CharEntry *character);

protected:
	bool makeShadow_;

	FT_Library library_;

	// The Object In Which FreeType Holds Information On A Given
	// Font Is Called A "face".
	FT_Face face_;
};

#endif /* _GLFONT2DFREETYPE_H_ */
