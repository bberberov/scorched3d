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

#include <water/Water2Patches.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLFont2d.h>
#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLStateExtension.h>
#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>

Water2Patches::Water2Patches() : patches_(0), 
	size_(0), totalSize_(0), patchSize_(0), bufferObject_(0)
{
}

Water2Patches::~Water2Patches()
{
	delete [] patches_;
	patches_ = 0;
	delete bufferObject_;
	bufferObject_ = 0;
}

void Water2Patches::generate(Water2Points &heights, 
	unsigned int totalSize, unsigned int patchSize,
	float waterHeight)
{
	size_ = totalSize / patchSize;
	totalSize_= totalSize;
	patchSize_ = patchSize;
	if (!patches_) patches_ = new Water2Patch[size_ * size_];

	// Generate all of the patches
	int i=0;
	for (int y=0; y<size_; y++)
	{
		for (int x=0; x<size_; x++, i++)
		{
			patches_[i].generate(heights, patchSize, int(totalSize), x , y, waterHeight);
		}
	}

	// Generate the VBO if any
	if (GLStateExtension::hasVBO())
	{
		int patchVolume = (patchSize + 1) * (patchSize + 1);
		int bufferSizeBytes = patchVolume * sizeof(Water2Patch::Data);
		int allBuffersSizeBytes = bufferSizeBytes * i;

		if (!bufferObject_ || bufferObject_->get_map_size() != allBuffersSizeBytes) 
		{
			delete bufferObject_;
			bufferObject_ = new GLVertexBufferObject();
			bufferObject_->init_data(allBuffersSizeBytes, 0, GL_STATIC_DRAW);
		}

		i=0;
		for (int y=0; y<size_; y++)
		{
			for (int x=0; x<size_; x++, i++)
			{
				bufferObject_->init_sub_data(i * bufferSizeBytes, bufferSizeBytes, patches_[i].getInternalData());
				patches_[i].setBufferOffSet(i * bufferSizeBytes);
			}
		}
	}

	// Generate the normal map
	generateNormalMap();
}

void Water2Patches::generateNormalMap()
{
	// compute texture data
	//fixme: for higher levels the computed data is not used, as mipmaps are generated
	//by glu. however this data should be much better!
	//but we can't feed it to texture class yet
	if (!normalMap_.getBits()) 
	{
		normalMap_ = Image(totalSize_, totalSize_);
	}

	unsigned char *normalBits = normalMap_.getBits();
	for (int y=0; y<totalSize_; y++) 
	{
		for (int x=0; x<totalSize_; x++, normalBits+=3) 
		{
			Water2Patch::Data *data = getPoint(x,y);
			normalBits[0] = (unsigned char)(data->nx*127.0f+128.0f);
			normalBits[1] = (unsigned char)(data->ny*127.0f+128.0f);
			normalBits[2] = (unsigned char)(data->nz*127.0f+128.0f);
		}
	}
}

Water2Patch *Water2Patches::getPatch(int index)
{
	return &patches_[index];
}

Water2Patch *Water2Patches::getPatch(int x, int y)
{
	int position = x + y * size_;
	DIALOG_ASSERT((position >= 0 && (position < size_ * size_)))
	return &patches_[position];
}

Water2Patch::Data *Water2Patches::getPoint(int x, int y)
{
	int nx = x % totalSize_;
	int ny = y % totalSize_;
	if (nx < 0) nx = totalSize_ + nx;
	if (ny < 0) ny = totalSize_ + ny;

	int px = nx / patchSize_;
	int py = ny / patchSize_;
	int sx = nx - (px * patchSize_);
	int sy = ny - (py * patchSize_);

	Water2Patch *patch = getPatch(px, py);
	return patch->getData(sx, sy);
}
