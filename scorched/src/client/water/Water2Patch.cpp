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

#include <water/Water2Patch.h>
#include <water/Water2Constants.h>
#include <common/Logger.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>

Water2Patch::Water2Patch() : 
	data_(0), dataSize_(0), size_(0), bufferOffSet_(-1)
{
}

Water2Patch::~Water2Patch()
{
	delete [] data_;
}

static Vector getPosition(Water2Points &heights, 
	int x, int y, int startX, int startY, int totalSize)
{
	int currentX = startX + x;
	int currentY = startY + y;
	int currentXNormalized = currentX % totalSize;
	int currentYNormalized = currentY % totalSize;
	if (currentXNormalized < 0) currentXNormalized += totalSize;
	if (currentYNormalized < 0) currentYNormalized += totalSize;

	int position = currentXNormalized + currentYNormalized * totalSize;
	DIALOG_ASSERT(position >= 0 && position < totalSize * totalSize);

	Vector &height = heights.getPoint(currentXNormalized, currentYNormalized);

	Vector result(
		height[0] + float(x) * 2.0f,
		height[1] + float(y) * 2.0f,
		height[2]);
	return result;
}

void Water2Patch::generate(Water2Points &heights,
	int size, int totalSize,
	int posX, int posY,
	float waterHeight)
{
	size_ = size;
	dataSize_ = (size + 1) * (size + 1);
	if (!data_) data_ = new Data[dataSize_];

	int startX = posX * size;  
	int startY = posY * size;

	{
		Data *data = data_;
		for (int y=0; y<=size; y++)
		{
			for (int x=0; x<=size; x++, data++)
			{
				// Calculate the position
				Vector position = getPosition(heights, x, y, startX, startY, totalSize);

				// Set the position
				data->x = position[0];
				data->y = position[1];
				data->z = position[2];
			}
		}

		data = data_;
		for (int y=0; y<=size; y++)
		{
			for (int x=0; x<=size; x++, data++)
			{
				// Calculate the normal
				Vector current(data->x, data->y, data->z);
				Vector other1, other2, other3, other4;

				if (x<size) other1 = Vector((data+1)->x, (data+1)->y, (data+1)->z);
				else other1 = getPosition(heights, x + 1, y, startX, startY, totalSize);
				
				if (y<size) other2 = Vector((data+size+1)->x, (data+size+1)->y, (data+size+1)->z);
				else other2 = getPosition(heights, x, y + 1, startX, startY, totalSize);

				if (x>0) other3 = Vector((data-1)->x, (data-1)->y, (data-1)->z);
				else other3 = getPosition(heights, x - 1, y, startX, startY, totalSize);

				if (y>0) other4 = Vector((data-size-1)->x, (data-size-1)->y, (data-size-1)->z);
				else other4 = getPosition(heights, x, y - 1, startX, startY, totalSize);

				Vector dir1 = other1 - current;
				Vector dir2 = other2 - current;
				Vector dir3 = other3 - current;
				Vector dir4 = other4 - current;

				Vector normal1 = dir1 * dir2;
				Vector normal3 = dir3 * dir4;

				Vector normal = (normal1 + normal3).Normalize();

				// Set the normal
				data->nx = normal[0];
				data->ny = normal[1];
				data->nz = normal[2];
			}
		}
	}
}

void Water2Patch::draw(MipMapPatchIndex &index)
{
	// Number triangles
	GLInfo::addNoTriangles(index.getSize());

	if (!OptionsDisplay::instance()->getNoGLDrawElements() &&
		GLStateExtension::hasDrawRangeElements())
	{
		// Map indices to draw
		unsigned short *indices = 0;
		if (index.getBufferOffSet() != -1)
		{
			indices = (unsigned short *) NULL + (index.getBufferOffSet() / sizeof(unsigned short));
		}
		else
		{
			indices = index.getIndices();
		}

		// Draw elements
		glDrawRangeElements(GL_TRIANGLE_STRIP, 
			index.getMinIndex(), 
			index.getMaxIndex(),
			index.getSize(), 
			GL_UNSIGNED_SHORT, 
			indices);
		DIALOG_ASSERT((index.getMaxIndex()-index.getMinIndex()+1) < 
			GLStateExtension::getMaxElementVertices());
		DIALOG_ASSERT(index.getSize() < 
			GLStateExtension::getMaxElementIndices());
	}
	else
	{
		glBegin(GL_TRIANGLE_STRIP);
			for (int i=0; i<index.getSize(); i++)
			{
				float *data = &data_[0].x + 
					(sizeof(Data) / 4 * index.getIndices()[i]);

				glNormal3fv(data + 3);
				glVertex3fv(data);
			}
		glEnd();
	}
}

Water2Patch::Data *Water2Patch::getData(int x, int y)
{
	DIALOG_ASSERT(x >= 0 && y >= 0 &&
		x <= size_ && y <= size_);
	return &data_[x + y * (size_ + 1)];
}
