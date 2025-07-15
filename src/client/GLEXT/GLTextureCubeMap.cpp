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

#include <common/Defines.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLTextureCubeMap.h>
#include <string.h>

GLTextureCubeMap::GLTextureCubeMap()
{
	memset(cubeTexNum_, 0, sizeof(GLuint) * 6);
}

GLTextureCubeMap::~GLTextureCubeMap()
{
	if (textureValid())
	{
		glDeleteTextures(6, cubeTexNum_);
		memset(cubeTexNum_, 0, sizeof(GLuint) * 6);
	}
}

bool GLTextureCubeMap::textureValid()
{
	return (glIsTexture(cubeTexNum_[0]) == GL_TRUE);
}

void GLTextureCubeMap::draw(bool force)
{
	if ((this != lastBind_) || force)
	{
		lastBind_ = this;
	}
}

bool GLTextureCubeMap::create(Image &bitmap, 
			bool mipMap)
{
	GLenum format = 
		(bitmap.getComponents()==1)?GL_LUMINANCE:
		((bitmap.getComponents() == 3)?GL_RGB:GL_RGBA);

	bool success = create(bitmap.getBits(),
			bitmap.getWidth(), 
			bitmap.getHeight(), 
			bitmap.getComponents(), 
			bitmap.getAlignment(),
			format,
			mipMap);

	return success;
}

bool GLTextureCubeMap::create(const void *data, 
					   GLint width, 
					   GLint height, 
					   GLint components, 
					   GLint alignment,
					   GLenum format, 
					   bool mipMap)
{
	bool success = false;
	if (data)
	{
		if (!textureValid())
		{
			glGenTextures(6, cubeTexNum_);
		}

		// Generate a texture GL_TEXTURE_CUBE_MAP_EXT and bind it
		glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, cubeTexNum_[0]);

		success = createTexture(data, width, height, components, alignment, format, mipMap);
	}

	return success;
}

bool GLTextureCubeMap::createTexture(const void * data, 
						GLint width, 
						GLint height, 
						GLint components, 
						GLint alignment,
						GLenum format, 
						bool mipMap)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GLenum texDefines[] = { 
		GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT};

	// Load all six cube maps
	for (int i=0; i<6; i++)
	{
		// Build the texture
		if (mipMap)
		{
			// Texture parameters
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			gluBuild2DMipmaps(texDefines[i], components, width, 
					height, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			// Texture parameters
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(texDefines[i], 0, components, width, 
				height, 0, format, GL_UNSIGNED_BYTE, data);
		}
	}

	return true;
}
