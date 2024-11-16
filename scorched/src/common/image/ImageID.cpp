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

#include <image/ImageID.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

ImageID::ImageID() :
	imageLocation_(S3D::eInvalidLocation)
{
}

ImageID::ImageID(S3D::FileLocation imageLocation,
	const std::string &imageName,
	const std::string &alphaName,
	bool invert) :
	imageLocation_(imageLocation),
	imageName_(imageName),
	alphaName_(alphaName),
	invert_(invert)
{
}

ImageID::~ImageID()
{
}

bool ImageID::initFromString(
	S3D::FileLocation imageLocation,
	const std::string &imageName,
	const std::string &alphaName,
	bool invert)
{
	imageLocation_ = imageLocation;
	imageName_ = imageName;
	alphaName_ = alphaName;
	invert_ = invert;

	return true;
}

bool ImageID::initFromNode(XMLNode *imageNode)
{
	bool invertTmp = false;
	std::string imageNameTmp, alphaNameTmp;
	imageNode->getNamedChild("image", imageNameTmp, false);
	imageNode->getNamedChild("alpha", alphaNameTmp, false);
	imageNode->getNamedChild("invert", invertTmp, false);

	S3D::FileLocation imageLocationTmp = S3D::eModLocation;
	std::string location = "mod";
	imageNode->getNamedChild("location", location, false);
	if (location == "mod") imageLocationTmp = S3D::eModLocation;
	else if (location == "data") imageLocationTmp = S3D::eDataLocation;
	else S3D::dialogExit("ImageID", S3D::formatStringBuffer("Unknown location %s", location.c_str()));

	initFromString(imageLocationTmp, imageNameTmp, alphaNameTmp, invertTmp);

	if (!imageName_.empty()) 
	{
		std::string location = getLocation(imageLocation_, imageName_);
		if (!S3D::fileExists(location))
		{
			S3D::dialogExit("ImageID", 
				S3D::formatStringBuffer(
				"Image file \"%s\" does not exist",
				location.c_str()));
		}
	}
	if (!alphaName_.empty()) 
	{
		std::string location = getLocation(imageLocation_, alphaName_);
		if (!S3D::fileExists(location))
		{
			S3D::dialogExit("ImageID", 
				S3D::formatStringBuffer(
				"Alpha file \"%s\" does not exist",
				location.c_str()));
		}
	}

	return imageNode->failChildren();
}

const std::string &ImageID::getStringHash()
{ 
	hash_ = imageName_ + "-" + alphaName_ + (invert_?"A":"B") + char(imageLocation_);
	return hash_; 
}
