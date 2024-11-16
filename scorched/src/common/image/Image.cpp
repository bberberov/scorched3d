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

#include <image/Image.h>
#include <image/ImageFactory.h>
#include <common/DefinesAssert.h>
#include <SDL/SDL.h>
#include "string.h"

Image::Image()
{
	data_ = new ImageData();
	data_->reference();
}

Image::Image(int width, int height, bool alpha)
{
	data_ = new ImageData(width, height, alpha?4:3, 255);
	data_->reference();
}

Image::Image(int width, int height, int components, unsigned char fill)
{
	data_ = new ImageData(width, height, components, fill);
	data_->reference();
}

Image::Image(const Image &other)
{
	data_ = other.data_;
	data_->reference();
}

Image::~Image()
{
	data_->dereference();
}

Image &Image::operator=(const Image &other)
{
	data_->dereference();
	data_ = other.data_;
	data_->reference();

	return *this;
}

unsigned char *Image::getBitsPos(int x, int y)
{
	DIALOG_ASSERT(x >= 0 && y >= 0 && x < getWidth() && y < getHeight());
	return &getBits()[(x * getComponents()) + (y * getWidth() * getComponents())];
}

unsigned char *Image::getBitsOffset(int offset)
{
	DIALOG_ASSERT(offset >=0 && offset < getComponents() * getWidth() * getHeight());
	return &getBits()[offset];
}

bool Image::writeToFile(const std::string &filename)
{
	if (!getBits() || getComponents() == 4) return false;

	unsigned char *brgbits = new unsigned char[getWidth() * getHeight() * 3];

	// Convert the returned bits from RGB to BGR
	// and flip the verticle scan lines
	unsigned char *from = (unsigned char *) getBits();
	for (int i=0; i<getHeight(); i ++)
	{
		unsigned char *destRow = ((unsigned char *) brgbits) + 
			((getHeight() - i - 1) * (getWidth() * 3));
		for (int j=0; j<getWidth(); j++)
		{
			unsigned char *dest = destRow + (j * getComponents());

			dest[0] = from[0];
			dest[1] = from[1];
			dest[2] = from[2];
			from+=getComponents();
		}
	}

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    Uint32 rmask, gmask, bmask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
#endif

	SDL_Surface *image = SDL_CreateRGBSurface( 
		SDL_SWSURFACE, getWidth(), getHeight(), 24, rmask, gmask, bmask, 0);
	memcpy(image->pixels, brgbits, getWidth() * getHeight() * 3);

	if (!image) return false;

	SDL_SaveBMP(image, filename.c_str());
    SDL_FreeSurface (image);

	delete [] brgbits;
	return true;
}

#ifndef S3D_SERVER

#include <GLEXT/GLState.h>
#include <common/Defines.h>

Image Image::createAlphaMult(float mult)
{
	if (getComponents() != 4) return Image();
	
	Image map(getWidth(), getHeight(), true);

	unsigned char *srcBits = getBits();
	unsigned char *destBits = map.getBits();
	for (int y=0; y<getHeight(); y++)
	{
		for (int x=0; x<getWidth(); x++, srcBits += 3, destBits += 3)
		{
			float a = float(srcBits[3]) * mult;
			a = MAX(a, 255);
			a = MIN(a, 0);

			destBits[0] = srcBits[0];
			destBits[1] = srcBits[1];
			destBits[2] = srcBits[2];
			destBits[3] = (unsigned char)(a);
		}
	}

	return map;
}

Image Image::createResize(int newWidth, int newHeight)
{
	if (!getBits()) return Image();

	Image map(newWidth, newHeight, getComponents(), 0);

	// Odd hack to fix a seeming memory corruption with gluScaleImage
	map.setBits(new unsigned char[newWidth * 2 * newHeight * map.getComponents()]);

	if (getWidth() != newWidth || getHeight() != newHeight)
	{
		GLenum format = GL_RGBA;
		if (getComponents() == 4) format = GL_RGBA;
		else if (getComponents() == 3) format = GL_RGB;
		else if (getComponents() == 2) format = GL_LUMINANCE_ALPHA;
		else if (getComponents() == 1) format = GL_LUMINANCE;

		int result = gluScaleImage(
			format, 
			getWidth(), getHeight(),
			GL_UNSIGNED_BYTE, getBits(),
			newWidth, newHeight, 
			GL_UNSIGNED_BYTE, map.getBits());
		if (result != 0)
		{
			const char *error = (const char *) gluErrorString(result);
			S3D::dialogExit("gluScaleImage", error);
		}
	}
	else
	{
		memcpy(map.getBits(), getBits(), getComponents() * getWidth() * getHeight());
	}

	return map;
}
#endif
