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

#include <landscape/GraphicalLandscapeMap.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLVertexBufferObject.h>

GraphicalLandscapeMap::GraphicalLandscapeMap() : 
	heightData_(0), bufferObject_(0), 
	width_(0), height_(0), bufferSizeBytes_(0)
{
}

GraphicalLandscapeMap::~GraphicalLandscapeMap()
{
	delete [] heightData_;
	heightData_ = 0;
	delete bufferObject_;
	bufferObject_ = 0;
}

void GraphicalLandscapeMap::create(const int width, const int height)
{
	width_ = width; 
	height_ = height;

	delete [] heightData_;
	heightData_ = new HeightData[(width_ + 1) * (height_ + 1)];
	int patchVolume = (width_ + 1) * (height_ + 1);
	bufferSizeBytes_ = patchVolume * sizeof(GraphicalLandscapeMap::HeightData);

	if (GLStateExtension::hasVBO())
	{
		if (!bufferObject_ || bufferObject_->get_map_size() != bufferSizeBytes_) 
		{
			delete bufferObject_;
			bufferObject_ = new GLVertexBufferObject();
			bufferObject_->init_data(bufferSizeBytes_, heightData_, GL_STATIC_DRAW);
		}
	}

	reset();
}

void GraphicalLandscapeMap::reset()
{
	HeightData *current = heightData_;

	int texTileX = width_ / 16;
	int texTileY = height_ / 16;

	for (int y=0; y<=height_; y++)
	{
		for (int x=0; x<=width_; x++)
		{
			current->floatPosition[0] = float(x);
			current->floatPosition[1] = float(y);
			current->floatPosition[2] = 0.0f;

			current->floatNormal[0] = 0.0f;
			current->floatNormal[1] = 0.0f;
			current->floatNormal[2] = 1.0f;

			current->texCoord1x = float(x) / float(width_);
			current->texCoord1y = float(y) / float(height_);

			current->texCoord2x = float(x) / float(width_) * float(texTileX);
			current->texCoord2y = float(y) / float(height_) * float(texTileY);

			current++;
		}
	}
}

void GraphicalLandscapeMap::setHeight(int w, int h, float height)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=height_);

	HeightData &data = heightData_[(width_+1) * h + w];
	data.floatPosition[2] = height;
}

void GraphicalLandscapeMap::setNormal(int w, int h, Vector &normal)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=height_);

	HeightData &data = heightData_[(width_+1) * h + w];
	data.floatNormal = normal;
}

void GraphicalLandscapeMap::updateWholeBuffer()
{
	// Generate the VBO if any
	if (!GLStateExtension::hasVBO()) return;

	bufferObject_->init_data(bufferSizeBytes_, heightData_, GL_STATIC_DRAW);
}
