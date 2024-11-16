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

#if !defined(AFX_ImageData_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
#define AFX_ImageData_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_

#include <string>

class ImageData  
{
public:
	ImageData();
	ImageData(int width, int height, int components = 3, unsigned char fill = 255);
	~ImageData();

	void clear();

	bool getLossless() { return lossless_; }
	unsigned char *getBits() { return bits_; }
	int getWidth() { return width_; }
	int getHeight() { return height_; }
	int getAlignment() { return alignment_; }
	int getComponents() { return components_; }

	void setLossless(bool lossless) { lossless_ = lossless; }
	void setBits(unsigned char *bits) { delete [] bits_; bits_ = bits; }
	void setWidth(int width) { width_ = width; }
	void setHeight(int height) { height_ = height; }
	void setAlignment(int alignment) { alignment_ = alignment; }
	void setComponents(int components) { components_ = components; }

	void reference();
	void dereference();

protected:
	bool lossless_;
	int referenceCount_;
	unsigned char *bits_;
	int width_;
	int height_;
	int alignment_;
	int components_;

	void createBlankInternal(int width, int height, int components, unsigned char fill);

private:
	ImageData(const ImageData &other);
	ImageData &operator=(ImageData &other);
};

#endif // !defined(AFX_ImageData_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
