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

#include <geomipmap/MipMapPatchIndexs.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <graph/OptionsDisplay.h>
#include <common/Logger.h>

MipMapPatchIndexs::IndexLevel::IndexLevel()
{
}

MipMapPatchIndexs::IndexLevel::~IndexLevel()
{
	while (!borderIndexs_.empty())
	{
		MipMapPatchIndex *index = borderIndexs_.back();
		borderIndexs_.pop_back();
		delete index;
	}
}

MipMapPatchIndexs::MipMapPatchIndexs() : 
	bufferObject_(0)
{
}

MipMapPatchIndexs::~MipMapPatchIndexs()
{
	while (!levels_.empty())
	{
		IndexLevel *level = levels_.back();
		levels_.pop_back();
		delete level;
	}
	delete bufferObject_;
	bufferObject_ = 0;
}

void MipMapPatchIndexs::generate(int size, int totalsize, unsigned int totallods)
{
	while (!levels_.empty())
	{
		IndexLevel *level = levels_.back();
		levels_.pop_back();
		delete level;
	}

	unsigned int totalVerts = 0;
	for (int lod=1; lod<=size; lod*=2)
	{
		IndexLevel *level = new IndexLevel();
		levels_.push_back(level);

		// borders
		for (unsigned int borders=0; borders<=4095; borders++)
		{
			MipMapPatchIndex *index = new MipMapPatchIndex();
			index->generate(size, totalsize, lod, borders, totallods);
			level->borderIndexs_.push_back(index);
			if (index->getIndices())
			{
				totalVerts += index->getSize();
			}
		}
	}

	int totalBufferSizeBytes = totalVerts * sizeof(unsigned short);
	Logger::log(S3D::formatStringBuffer(
		"Index Memory Size : %u bytes", totalBufferSizeBytes));

	// Store this array in a vertex buffer (if available)
	if (GLStateExtension::hasVBO())
	{
		if (!bufferObject_ || bufferObject_->get_map_size() != totalBufferSizeBytes) 
		{
			delete bufferObject_;
			bufferObject_ = new GLVertexBufferObject(true);
			bufferObject_->init_data(totalBufferSizeBytes, 0, GL_STATIC_DRAW);
		}

		unsigned int offsetBytes = 0;
		int i = 0;
		for (int lod=1; lod<=size; lod*=2, i++)
		{
			IndexLevel *level = levels_[i];
			for (unsigned int borders=0; borders<=4095; borders++)
			{
				MipMapPatchIndex *index = level->borderIndexs_[borders];
				if (index->getIndices())
				{
					unsigned int bufferSizeBytes = index->getSize() * sizeof(unsigned short);
					bufferObject_->init_sub_data(offsetBytes, bufferSizeBytes, index->getIndices());
					index->setBufferOffSet(offsetBytes);
					offsetBytes += bufferSizeBytes;
				}
			}
		}
	}
}
