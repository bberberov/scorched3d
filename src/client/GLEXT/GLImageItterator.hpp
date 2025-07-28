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

#if !defined(AFX_ImageITTERATOR_H__F83D9F30_02DF_44EC_B2E7_1F4D87D6FBC8__INCLUDED_)
#define AFX_ImageITTERATOR_H__F83D9F30_02DF_44EC_B2E7_1F4D87D6FBC8__INCLUDED_

#include <image/Image.h>

/**
A helper class that simplifes the iteration (looping) through
the components contained within bitmap files.

	ImageItterator bitmapItor(myBitmap,
		myRequiredWidth, myRequiredHeight, myRequiredType);
	for (int by=0; by<myRequiredHeight; by++, bitmapItor.incY())
	{
		for (int bx=0; bx<myRequiredWidth; bx++, bitmapItor.incX())
		{
			// Get the bytes at bx, by position in the bitmap.
			// This will be an array of 3 or 4 bytes depending
			// on the bitmap type.
			unsigned char *sourceBits = bitmapItor.getPos();
		}
	}
*/
class ImageItterator  
{
public:
	/**
	Defines the type of the iteration.
	Wrap - The bitmap is tiled/repeated if it is smaller
	than the given destX/destY sizes.
	Stretch - The bitmap extents are stretched to
	match the destX and destY sizes.
	*/
	enum Type
	{
		wrap,
		stretch
	};

	/**
	DestX is the maximum width you will ask the iterator to return.
	DestY is the maximum height you will ask the iterator to return.
	Type defines how these width/height maps the bitmaps size.
	*/
	ImageItterator(Image &bitmap, 
						int destX,
						int destY,
						Type type = stretch);
	virtual ~ImageItterator();

	/**
	Reset the iterator.
	*/
	void reset();
	/**
	Position the iterator at the next bitmap bytes in the row.
	*/
	void incX();
	/**
	Position the iterator at the next row.
	*/
	void incY();
	/**
	Get the bytes at the current iterator's position.
	*/
	unsigned char *getPos();

	int getComponents() { return components_; }

protected:
	Image &bitmap_;
	float dx_, dy_;
	int width_, components_;
	Type type_;

	unsigned char *pos_;
	float posX_, posY_;
};

#endif // !defined(AFX_ImageITTERATOR_H__F83D9F30_02DF_44EC_B2E7_1F4D87D6FBC8__INCLUDED_)
