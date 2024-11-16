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

#if !defined(__INCLUDE_TextureReferenceDatah_INCLUDE__)
#define __INCLUDE_TextureReferenceDatah_INCLUDE__

#include <image/ImageID.h>
#include <GLEXT/GLTexture.h>

class GLTextureReferenceData
{
public:
	GLTextureReferenceData(const ImageID &imageId, unsigned texState);
	~GLTextureReferenceData();
	
	void reset();

	int incrementReferenceCount() { return ++referenceCount_; }
	int decrementReferenceCount() { return --referenceCount_; }

	ImageID &getImageID() { return imageId_; }
	unsigned int getTexState() { return texState_; }
	GLTexture *getTexture();
protected:
	int referenceCount_;
	GLTexture *texture_;
	ImageID imageId_;
	unsigned texState_;
private:
	GLTextureReferenceData(const GLTextureReferenceData &other);
	GLTextureReferenceData &operator=(const GLTextureReferenceData &other);
};

#endif
