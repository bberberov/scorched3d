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

#if !defined(__INCLUDE_MipMapPatchIndexsh_INCLUDE__)
#define __INCLUDE_MipMapPatchIndexsh_INCLUDE__

#include <geomipmap/MipMapPatchIndex.h>
#include <common/DefinesAssert.h>
#include <common/DefinesMath.h>
#include <vector>

class GLVertexBufferObject;
class MipMapPatchIndexs
{
protected:

	class IndexLevel // One per LOD
	{
	public:
		IndexLevel();
		~IndexLevel();

		std::vector<MipMapPatchIndex *> borderIndexs_;
	};

public:
	MipMapPatchIndexs();
	~MipMapPatchIndexs();

	MipMapPatchIndex *getIndex(int lod, int leftLod, int rightLod, int topLod, int bottomLod, int addLod = 0)
	{
		lod = MIN(lod + addLod, getNoLevels() - 1);
		leftLod = MIN(leftLod + addLod, getNoLevels() - 1);
		rightLod = MIN(rightLod + addLod, getNoLevels() - 1);
		topLod = MIN(topLod + addLod, getNoLevels() - 1);
		bottomLod = MIN(bottomLod + addLod, getNoLevels() - 1);

		unsigned int borders = 0;
		if (leftLod != -1 && leftLod > lod) 
		{
			borders |= (leftLod - lod);
		}
		if (rightLod != -1 && rightLod > lod)
		{
			borders |= (rightLod - lod) << 3;
		}
		if (topLod != -1 && topLod > lod) 
		{
			borders |= (topLod - lod) << 9;
		}
		if (bottomLod != -1 && bottomLod > lod) 
		{
			borders |= (bottomLod - lod) << 6;
		}

		return getIndex(lod, borders);
	}

	MipMapPatchIndex *getIndex(int lod, int border) 
	{ 
		if (lod<0) lod=0;
		else if (lod >= getNoLevels()) lod = getNoLevels()-1;

		DIALOG_ASSERT(border < 4096);

		IndexLevel *level = levels_[lod];
		MipMapPatchIndex *index = level->borderIndexs_[border];
		if (!index->getIndices()) return 0;

		return index;
	}
	int getNoLevels() { return (int) levels_.size(); }

	void generate(int size, int totalsize, unsigned int totallods = 99);

	GLVertexBufferObject *getBufferObject() { return bufferObject_; }

protected:
	std::vector<IndexLevel *> levels_;
	GLVertexBufferObject *bufferObject_;
};

#endif // __INCLUDE_MipMapPatchIndexsh_INCLUDE__
