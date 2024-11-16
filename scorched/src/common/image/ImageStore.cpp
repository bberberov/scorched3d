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

#include <image/ImageStore.h>
#include <image/ImageFactory.h>
#include <common/Defines.h>

ImageStore *ImageStore::instance_ = 0;

ImageStore *ImageStore::instance()
{
	if (!instance_)
	{
		instance_ = new ImageStore;
	}
	return instance_;
}

ImageStore::ImageStore()
{
}

ImageStore::~ImageStore()
{
}

Image ImageStore::loadImage(ImageID &imageId)
{
	std::map<std::string, Image *>::iterator findItor =
		imageMap_.find(imageId.getStringHash());
	if (findItor == imageMap_.end())
	{
		Image image = ImageFactory::loadImageID(imageId);
		imageMap_[imageId.getStringHash()] = new Image(image);
		return image;
	}
	return *findItor->second;
}
