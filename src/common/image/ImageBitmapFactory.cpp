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

#include <image/ImageBitmapFactory.h>
#include <common/Defines.h>
#include <SDL/SDL.h>

Image ImageBitmapFactory::loadFromFile(const char * filename, bool alpha)
{
	SDL_Surface *image = SDL_LoadBMP(filename);
	if (!image) return Image();

	if (image->format->BitsPerPixel != 24)
	{
		S3D::dialogExit("ImageBitmap",
			S3D::formatStringBuffer("ERROR: Bitmap \"%s\" is not encoded as a 24bit bitmap",
				filename));
	}

	// Create the internal byte array
	Image result(image->w, image->h, alpha);

	// Convert the returned bits from BGR to RGB
	// and flip the verticle scan lines
	unsigned char *from = (unsigned char *) image->pixels;
	for (int i=0; i<result.getHeight(); i ++)
	{
		unsigned char *destRow = ((unsigned char *) result.getBits()) + 
			((result.getHeight() - i - 1) * (result.getWidth() * result.getComponents()));
		for (int j=0; j<result.getWidth(); j++)
		{
			unsigned char *dest = destRow + (j * result.getComponents());

			dest[0] = from[2];
			dest[1] = from[1];
			dest[2] = from[0];
			if (alpha)
			{
				dest[3] = (unsigned char)(from[0]+from[1]+from[2]==0?0:255);
			}
			
			from+=3;
		}
	}
	
	SDL_FreeSurface(image);
	return result;
}
