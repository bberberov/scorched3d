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
#include <GLEXT/GLTextureSet.h>

GLTextureSet::GLTextureSet()
{
}

GLTextureSet::~GLTextureSet()
{
	while (!textures_.empty())
	{
		GLTexture *texture = textures_.back();
		delete texture;
		textures_.pop_back();
	}
}

int GLTextureSet::getNoTextures()
{
	return (int) textures_.size();
}

GLTexture *GLTextureSet::getTexture(int index)
{
	DIALOG_ASSERT(index >= 0 && index < getNoTextures());

	return textures_[index];
}

void GLTextureSet::addTexture(GLTexture *texture)
{
	textures_.push_back(texture);
}
