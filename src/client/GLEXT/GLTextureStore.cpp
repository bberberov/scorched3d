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

#include <GLEXT/GLTextureStore.h>

GLTextureStore *GLTextureStore::instance_ = 0;

GLTextureStore *GLTextureStore::instance()
{
	if (!instance_)
	{
		instance_ = new GLTextureStore;
	}
	return instance_;
}

GLTextureStore::GLTextureStore()
{
}

GLTextureStore::~GLTextureStore()
{
}

void GLTextureStore::resetModFiles()
{
	std::map<std::string, GLTextureReferenceData *>::iterator itor;
	for (itor = references_.begin();
		itor != references_.end();
		++itor)
	{
		if (itor->second->getImageID().getImageLocation() == S3D::eModLocation)
		{
			itor->second->reset();
		}
	}
}

GLTextureReferenceData *GLTextureStore::getTextureReference(const ImageID &imageId, unsigned texState)
{
	std::string hash = ((ImageID &)imageId).getStringHash() + char(texState);
	std::map<std::string, GLTextureReferenceData *>::iterator itor = references_.find(hash);
	if (itor != references_.end())
	{
		return itor->second;
	}

	GLTextureReferenceData *result = new GLTextureReferenceData(imageId, texState);
	result->incrementReferenceCount();
	references_[hash] = result;
	return result;
}

void GLTextureStore::removeTextureReference(GLTextureReferenceData *reference)
{
	if (reference->incrementReferenceCount() <= 0)
	{
		std::string hash = ((ImageID &)reference->getImageID()).getStringHash() + char(reference->getTexState());
		references_.erase(hash);
		delete reference;
	}
}
