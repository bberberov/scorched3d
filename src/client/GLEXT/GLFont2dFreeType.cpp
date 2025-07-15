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

#include <GLEXT/GLFont2dFreeType.h>
#include <GLEXT/GLState.h>
#include <common/Defines.h>

// This Function Gets The First Power Of 2 >= The
// Int That We Pass It.
static inline int next_p2 (int a )
{
	int rval=1;
	// rval<<=1 Is A Prettier Way Of Writing rval*=2; 
	while(rval<a) rval<<=1;
	return rval;
}

GLFont2dFreeType::GLFont2dFreeType() : makeShadow_(false)
{

}

GLFont2dFreeType::~GLFont2dFreeType()
{
	// We Don't Need The Face Information Now That The Display
	// Lists Have Been Created, So We Free The Assosiated Resources.
	FT_Done_Face(face_);

	// Ditto For The Font Library.
	FT_Done_FreeType(library_);
}


bool GLFont2dFreeType::createFont(const std::string &typeFace, unsigned int h, bool makeShadow)
{
	// Create And Initilize A FreeType Font Library.
	if (FT_Init_FreeType( &library_ )) 
	{
		S3D::dialogMessage("GLFont2d", "FT_Init_FreeType failed"); 
		return false;
	}

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face( library_, typeFace.c_str(), 0, &face_ )) 
	{
		S3D::dialogMessage("GLFont2d", S3D::formatStringBuffer(
			"FT_New_Face failed (there is probably a problem with your font file \"%s\")",
			typeFace.c_str())); 
		return false;
	}

	// For Some Twisted Reason, FreeType Measures Font Size
	// In Terms Of 1/64ths Of Pixels.  Thus, To Make A Font
	// h Pixels High, We Need To Request A Size Of h*64.
	// (h << 6 Is Just A Prettier Way Of Writing h*64)
	FT_Set_Char_Size( face_, h << 6, h << 6, 96, 96);

	makeShadow_ = makeShadow;

	return true;
}

bool GLFont2dFreeType::createCharacter(unsigned int ch, GLFont2dStorage::CharEntry *character) 
{
	// The First Thing We Do Is Get FreeType To Render Our Character
	// Into A Bitmap.  This Actually Requires A Couple Of FreeType Commands:

	/*
	char buffer[256];
	for (int i=2588; i<5000; i++) {
		FT_Get_Glyph_Name(face_, i, buffer, 256);
		if (0 == strcmp(buffer, "filledbox")) {
			break;
		}
	}
	*/

	// Load The Glyph For Our Character.
 	FT_UInt charIndex = FT_Get_Char_Index( face_, ch );
	if (!charIndex) 
	{
		if (charIndex < (FT_UInt) face_->num_glyphs) charIndex = ch;
		else charIndex = FT_Get_Char_Index( face_, '?');
	}

	if(FT_Load_Glyph( face_, charIndex, FT_LOAD_DEFAULT )) 
	{ 
		charIndex = FT_Get_Char_Index( face_, '?');
		if(FT_Load_Glyph( face_, charIndex, FT_LOAD_DEFAULT )) 
		{ 
			S3D::dialogMessage("GLFont", "FT_Load_Glyph failed"); return false; 
		}
	}

	// Move The Face's Glyph Into A Glyph Object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face_->glyph, &glyph ))
	{ 
		S3D::dialogMessage("GLFont", "FT_Get_Glyph failed"); return false; 
	}

	// Convert The Glyph To A Bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	// This Reference Will Make Accessing The Bitmap Easier.
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	// Get the width and height
	int bitmap_width = bitmap.width;
	int bitmap_rows = bitmap.rows;
	unsigned char *bitmap_buffer = bitmap.buffer;

	if (makeShadow_)
	{
		int shadow_width = bitmap_width + 4;
		int shadow_rows = bitmap_rows + 4;
		unsigned char *shadow_buffer = new unsigned char[
			shadow_width * shadow_rows];
		memset(shadow_buffer, 0, shadow_width * shadow_rows);
		
		for(int j=2; j<shadow_rows-2;j++) 
		{
			for(int i=2; i< shadow_width-2; i++)
			{
				int bi = i-2;
				int bj = j-2;
				unsigned char bitmapValue = bitmap_buffer[bi + bitmap_width*bj];
				
				for (int b=-2; b<=2; b++)
				{
					for (int a=-2; a<=2; a++)
					{
						int si = i+a;
						int sj = j+b;
						unsigned char &shadowValue = shadow_buffer[si + shadow_width*sj];

						int sv = int(shadowValue) + int(bitmapValue);
						if (sv > 255) sv = 255;
						shadowValue = (unsigned char) sv;
					}
				}
			}
		}

		bitmap_width = shadow_width;
		bitmap_rows = shadow_rows;
		bitmap_buffer = shadow_buffer;
	}

	// Use Our Helper Function To Get The Widths Of
	// The Bitmap Data That We Will Need In Order To Create
	// Our Texture.
	int width = next_p2( bitmap_width );
	int height = next_p2( bitmap_rows );

	// Allocate Memory For The Texture Data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height];

	// Here We Fill In The Data For The Expanded Bitmap.
	// Notice That We Are Using A Two Channel Bitmap (One For
	// Channel Luminosity And One For Alpha), But We Assign
	// Both Luminosity And Alpha To The Value That We
	// Find In The FreeType Bitmap. 
	// We Use The ?: Operator To Say That Value Which We Use
	// Will Be 0 If We Are In The Padding Zone, And Whatever
	// Is The FreeType Bitmap Otherwise.
	for(int j=0; j <height;j++) {
		for(int i=0; i < width; i++){
			expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] = 
				(i>=bitmap_width || j>=bitmap_rows) ?
				0 : bitmap_buffer[i + bitmap_width*j];
		}
	}

	if (makeShadow_) delete [] bitmap_buffer;

	// Now We Just Setup Some Texture Parameters.
	glGenTextures(1, &character->texture);
	glBindTexture( GL_TEXTURE_2D, character->texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
	// Here We Actually Create The Texture Itself, Notice
	// That We Are Using GL_LUMINANCE_ALPHA To Indicate That
	// We Are Using 2 Channel Data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	// With The Texture Created, We Don't Need The Expanded Data Anymore.
	delete [] expanded_data;

	// Now We Create The Display List
	character->displaylist = glGenLists(1);
	glNewList(character->displaylist, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, character->texture);
	glPushMatrix();

	// First We Need To Move Over A Little So That
	// The Character Has The Right Amount Of Space
	// Between It And The One Before It.
	glTranslatef((float) bitmap_glyph->left, 0.0f, 0.0f);
	character->left = bitmap_glyph->left;

	// Now We Move Down A Little In The Case That The
	// Bitmap Extends Past The Bottom Of The Line 
	// This Is Only True For Characters Like 'g' Or 'y'.
	glTranslatef(0.0f,(float) bitmap_glyph->top-bitmap_rows,0.0f);
	character->rows = bitmap_glyph->top-bitmap_rows;

	// Now We Need To Account For The Fact That Many Of
	// Our Textures Are Filled With Empty Padding Space.
	// We Figure What Portion Of The Texture Is Used By 
	// The Actual Character And Store That Information In
	// The x And y Variables, Then When We Draw The
	// Quad, We Will Only Reference The Parts Of The Texture
	// That Contains The Character Itself.
	
	float x=(float)bitmap_width / (float)width;
	float y=(float)bitmap_rows / (float)height;
	character->x = x;
	character->y = y;

	// Here We Draw The Texturemapped Quads.
	// The Bitmap That We Got From FreeType Was Not 
	// Oriented Quite Like We Would Like It To Be,
	// But We Link The Texture To The Quad
	// In Such A Way That The Result Will Be Properly Aligned.
	character->width = (float)bitmap_width;
	character->height = (float)bitmap_rows;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex2f(0.0f,(float)bitmap_rows);
	glTexCoord2f(0.0f,y); glVertex2f(0.0f,0.0f);
	glTexCoord2f(x,y); glVertex2f((float)bitmap_width,0.0f);
	glTexCoord2f(x,0.0f); glVertex2f((float)bitmap_width,(float)bitmap_rows);
	glEnd();
	glPopMatrix();
	glTranslatef((float)(face_->glyph->advance.x >> 6) ,0.0f ,0.0f);

	character->advances = (face_->glyph->advance.x >> 6);

	// Increment The Raster Position As If We Were A Bitmap Font.
	// (Only Needed If You Want To Calculate Text Length)
	// ImageBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	// Finish The Display List
	glEndList();
	
	FT_Done_Glyph(glyph);
	return true;
}
