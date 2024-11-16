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

#include <wx/utils.h>
#include <wxdialogs/TrueTypeFont.h>
#include <common/Defines.h>

TrueTypeFont::TrueTypeFont(const std::string &typeFace, unsigned int h)
{
	// Create And Initilize A FreeType Font Library.
	if (FT_Init_FreeType(&library)) 
	{
		return;
	}

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face(library, typeFace.c_str(), 0, &face)) 
	{
		return;
	}
	
	// For Some Twisted Reason, FreeType Measures Font Size
	// In Terms Of 1/64ths Of Pixels.  Thus, To Make A Font
	// h Pixels High, We Need To Request A Size Of h*64.
	// (h << 6 Is Just A Prettier Way Of Writing h*64)
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

	// This Is Where We Actually Create Each Of The Fonts Characters
	for(unsigned char i=0;i<128;i++)
	{
		createCharacter(face, i);
	}
}

TrueTypeFont::~TrueTypeFont()
{
	// We Don't Need The Face Information Now That The Display
	// Lists Have Been Created, So We Free The Assosiated Resources.
	FT_Done_Face(face);

	// Ditto For The Font Library.
	FT_Done_FreeType(library);
}

bool TrueTypeFont::createCharacter(FT_Face face, unsigned char ch)
{
	// The First Thing We Do Is Get FreeType To Render Our Character
	// Into A Bitmap.  This Actually Requires A Couple Of FreeType Commands:

	// Load The Glyph For Our Character.
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT )) 
	{ 
		return false; 
	}

	// Move The Face's Glyph Into A Glyph Object.
	if(FT_Get_Glyph( face->glyph, &glyphs[ch] ))
	{ 
		return false; 
	}

	// Convert The Glyph To A Bitmap.
	FT_Glyph_To_Bitmap( &glyphs[ch], ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) glyphs[ch];

	// This Reference Will Make Accessing The Bitmap Easier.
	bitmaps[ch]= &bitmap_glyph->bitmap;

	return true;
}

bool TrueTypeFont::getImageForText(const std::string &text, wxImage &image)
{
	int width = 0;
	int height = 0;
	int topheight = 0;

	// Find out the total image height and width
	FT_GlyphSlot  slot = face->glyph; 
	for (const char *c=text.c_str(); *c; c++)
	{
		FT_Load_Char(face, *c, FT_LOAD_RENDER);
		FT_Bitmap &bitmap = slot->bitmap;

		width += slot->advance.x >> 6;
		if (bitmap.rows > height) height = bitmap.rows;
		if (bitmap.rows - slot->bitmap_top > topheight) 
			topheight = bitmap.rows - slot->bitmap_top;
	}

	// Create the image
	image.Create(width, height + topheight);

	// For each char
	int posx = 0;
	int posy = 0;
	for (const char *c=text.c_str(); *c; c++)
	{
		FT_Load_Char( face, *c, FT_LOAD_RENDER);
		FT_Bitmap &bitmap = slot->bitmap;

		int x = slot->bitmap_left;
		int y = height - slot->bitmap_top;

		for (int j=0; j<bitmap.rows; j++)
		{
			for (int i=0; i<bitmap.width; i++)
			{
				int rx = (i + posx + x);
				int ry = (j + posy + y);

				if (rx >= image.GetWidth() || rx < 0) continue;
				if (ry >= image.GetHeight() || ry < 0) continue;

				unsigned char *dest =
					image.GetData() + ((ry * image.GetWidth()) + rx) * 3;
				unsigned char src = bitmap.buffer[i + bitmap.width * j];

				src = (unsigned int) (float(src) * 0.9f);


				dest[0] = dest[1] = dest[2] = src;
			}
		}

		posx += slot->advance.x >> 6;
	}

	return true;
}
