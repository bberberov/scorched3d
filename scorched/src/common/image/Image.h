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

#if !defined(AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
#define AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_

#include <string>
#include <image/ImageData.h>

class Image  
{
public:
	Image();
	Image(int width, int height, bool alpha = false);
	Image(int width, int height, int components, unsigned char fill);
	Image(const Image &other);
	virtual ~Image();

	Image &operator=(const Image &other);

	unsigned char *getBits() { return data_->getBits(); }
	unsigned char *getBitsOffset(int offset);
	unsigned char *getBitsPos(int x, int y);

	bool getLossless() { return data_->getLossless(); }
	int getWidth() { return data_->getWidth(); }
	int getHeight() { return data_->getHeight(); }
	int getAlignment() { return data_->getAlignment(); }
	int getComponents() { return data_->getComponents(); }

	void setLossless(bool lossless) { data_->setLossless(lossless); }
	void setBits(unsigned char *bits) { data_->setBits(bits); }
	void setWidth(int width) { data_->setWidth(width); }
	void setHeight(int height) { data_->setHeight(height); }
	void setAlignment(int alignment) { data_->setAlignment(alignment); }
	void setComponents(int components) { data_->setComponents(components); }

	virtual bool writeToFile(const std::string &filename);

#ifndef S3D_SERVER
	Image createAlphaMult(float mult);
	Image createResize(int newWidth, int newHeight);
#endif
private:
	ImageData *data_;
};

#endif // !defined(AFX_Image_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
