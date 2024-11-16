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

#if !defined(__INCLUDE_Water2Patchesh_INCLUDE__)
#define __INCLUDE_Water2Patchesh_INCLUDE__

#include <water/Water2Patch.h>
#include <image/Image.h>
#include <GLEXT/GLTexture.h>

class GLVertexBufferObject;
class Water2Patches
{
public:
	Water2Patches();
	~Water2Patches();

	void generate(Water2Points &heights, 
		unsigned int totalSize, unsigned int patchSize,
		float waterHeight);

	Water2Patch *getPatch(int index);
	Water2Patch *getPatch(int x, int y);
	Water2Patch::Data *getPoint(int x, int y);
	int getSize() { return size_; }
	Image &getNormalMap() { return normalMap_; }
	GLTexture &getAOF() { return aof_; }
	GLVertexBufferObject *getBufferObject() { return bufferObject_; }

protected:
	int size_;
	int totalSize_, patchSize_;
	Image normalMap_;
	GLTexture aof_;
	GLVertexBufferObject *bufferObject_;
	Water2Patch *patches_;

	void generateNormalMap();
};

#endif // __INCLUDE_Water2Patchesh_INCLUDE__
