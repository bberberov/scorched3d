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

#if !defined(AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
#define AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_

#include <string>
#include <image/ImageID.h>
#include <image/Image.h>

class ImageFactory  
{
public:
	static Image loadImageID(
		const ImageID &imageId);
	static Image loadImage(
		S3D::FileLocation imageLocation,
		const std::string &filename, 
		const std::string &alphafilename = "", 
		bool invert = true);
	static Image loadAlphaImage(
		S3D::FileLocation imageLocation,
		const std::string &filename);

#ifndef S3D_SERVER
	static Image grabScreen();
#endif

protected:
	static Image loadImageInternal(S3D::FileLocation imageLocation, const std::string &filename, bool alphaName);
	static Image combineImage(Image file, Image alphaFile, bool invert);

private:
	ImageFactory();
};

#endif // !defined(AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
