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

#if !defined(__INCLUDE_DialogUtilsh_INCLUDE__)
#define __INCLUDE_DialogUtilsh_INCLUDE__

#include <wx/wx.h>
#include <wx/image.h>
#include <ft2build.h>
#include <string>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

class TrueTypeFont
{
public:
	TrueTypeFont(const std::string &typeFace, unsigned int h);
	virtual ~TrueTypeFont();

	bool getImageForText(const std::string &text, wxImage &image);

protected:
	FT_Face face;
	FT_Library library;
	FT_Glyph glyphs[128];
	FT_Bitmap *bitmaps[128];

	bool createCharacter(FT_Face face, unsigned char ch);
};

#endif
