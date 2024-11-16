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

#include <GLEXT/GLTextureReferenceData.h>
#include <GLEXT/GLTextureReference.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLTexture.h>

GLTextureReferenceData::GLTextureReferenceData(const ImageID &imageId, unsigned texState) :
	imageId_(imageId),
	texState_(texState),
	texture_(0),
	referenceCount_(0)
{
}

GLTextureReferenceData::~GLTextureReferenceData()
{
	reset();
}

void GLTextureReferenceData::reset()
{
	delete texture_;
	texture_ = 0;
}

GLTexture *GLTextureReferenceData::getTexture()
{
	if (!texture_)
	{
		Image image = ImageFactory::loadImageID(imageId_);
		texture_ = new GLTexture();
		texture_->create(image, texState_ & GLTextureReference::eMipMap);
		if (texState_ & GLTextureReference::eTextureClamped)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		}
	}
	return texture_;
}