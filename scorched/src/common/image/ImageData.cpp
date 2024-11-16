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

#include <image/ImageData.h>
#include <string.h>

ImageData::ImageData() :
	referenceCount_(0),
	bits_(0),
	width_(0),
	height_(0),
	alignment_(0),
	components_(0),
	lossless_(true)
{
}

ImageData::ImageData(int width, int height, int components, unsigned char fill) :
	lossless_(true),
	referenceCount_(0),
	bits_(0),
	width_(0),
	height_(0),
	alignment_(0),
	components_(0)
{
	createBlankInternal(width, height, components, fill);
}

ImageData::~ImageData()
{
	clear();
}

void ImageData::reference()
{
	referenceCount_++;
}

void ImageData::dereference()
{
	referenceCount_--;
	if (referenceCount_ <= 0) delete this;
}

void ImageData::clear()
{
	delete [] bits_;
	bits_ = 0;
	width_ = 0;
	height_ = 0;
	alignment_ = 0;
	components_ = 0;
	lossless_ = true;
}

void ImageData::createBlankInternal(int width, int height, int components, unsigned char fill)
{
	clear();
	lossless_ = true;
	width_ = width;
	height_ = height;
	components_ = components;
	int bitsize = components_ * width * height;

	bits_ = new unsigned char[bitsize];
	memset(bits_, fill, bitsize);
}
