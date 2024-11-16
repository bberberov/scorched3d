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

#if !defined(__INCLUDE_TextureReferenceh_INCLUDE__)
#define __INCLUDE_TextureReferenceh_INCLUDE__

#include <GLEXT/GLTextureReferenceData.h>

class GLTextureReference : public GLTextureBase
{
public:
	enum TextureState 
	{
		eMipMap = 1,
		eTextureClamped = 2
	};

	GLTextureReference();
	GLTextureReference(const GLTextureReference &other);
	GLTextureReference(const ImageID &imageId, unsigned texState = eMipMap);
	~GLTextureReference();

	GLTextureReference &operator=(const GLTextureReference &other);
	void setImageID(const ImageID &imageId, unsigned texState = eMipMap);

	virtual void draw(bool force = false) { data_->getTexture()->draw(force); }
	
	GLTextureReferenceData *getData() { return data_; }
	bool isValid() { return data_ != 0; }
	ImageID &getImageID() { return data_->getImageID(); }
	unsigned int getTexState() { return data_->getTexState(); }
	GLTexture *getTexture() { return data_->getTexture(); }
protected:
	GLTextureReferenceData *data_;
};

#endif
