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

#if !defined(__INCLUDE_ImageIDh_INCLUDE__)
#define __INCLUDE_ImageIDh_INCLUDE__

#include <string>
#include <common/DefinesFile.h>

class XMLNode;
class ImageID
{
public:
	ImageID();
	ImageID(S3D::FileLocation imageLocation,
		const std::string &imageName,
		const std::string &alphaName = "",
		bool invert = false);
	virtual ~ImageID();

	bool initFromNode(XMLNode *imageNode);

	bool initFromString(
		S3D::FileLocation imageLocation,
		const std::string &imageName,
		const std::string &alphaName = "",
		bool invert = false);

	bool isValid() { return imageLocation_ != S3D::eInvalidLocation; }
	S3D::FileLocation getImageLocation() { return imageLocation_; }
	const std::string &getStringHash();
	const std::string &getImageName() { return imageName_; }
	const std::string &getAlphaName() { return alphaName_; }
	bool getInvert() { return invert_; }

protected:
	S3D::FileLocation imageLocation_;
	std::string imageName_;
	std::string alphaName_;
	std::string hash_;
	bool invert_;
};

#endif // __INCLUDE_ImageIDh_INCLUDE__
