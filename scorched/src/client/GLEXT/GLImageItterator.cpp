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

#include <GLEXT/GLImageItterator.h>

ImageItterator::ImageItterator(Image &bitmap,
									int destX,
									int destY,
									Type type)
	: bitmap_(bitmap), type_(type)
{
	if (type == stretch)
	{
		dx_ = (float) bitmap.getWidth() / (float) destX;
		dy_ = (float) bitmap.getHeight() / (float) destY;
	}
	else
	{
		dx_ = 1;
		dy_ = 1;
	}
	width_ = bitmap.getComponents() * bitmap.getWidth();
	components_ = bitmap.getComponents();

	reset();
}

ImageItterator::~ImageItterator()
{

}

void ImageItterator::reset()
{
	posX_ = posY_ = 0.0f;
	pos_ = bitmap_.getBits();
}

void ImageItterator::incX()
{
	posX_ += dx_;
	if (posX_ >= bitmap_.getWidth()) posX_ = 0.0f;
}

void ImageItterator::incY()
{
	posX_ = 0.0f;
	posY_ += dy_;
	if (posY_ >= bitmap_.getHeight()) posY_ = 0.0f;
	pos_ = (unsigned char *) (bitmap_.getBits() + ((int) posY_  * width_));
}

unsigned char *ImageItterator::getPos()
{
	return pos_ + (int) posX_ * components_;
}
