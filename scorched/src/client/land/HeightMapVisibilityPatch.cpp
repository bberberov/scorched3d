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

#include <land/HeightMapVisibilityPatch.h>
#include <land/VisibilityPatchGrid.h>
#include <landscapemap/GraphicalHeightMap.h>
#include <landscape/GraphicalLandscapeMap.h>
#include <graph/OptionsDisplay.h>
#include <graph/MainCamera.h>
#include <common/Logger.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>
#include <GLW/GLWFont.h>

HeightMapVisibilityPatch::HeightMapVisibilityPatch(HeightMap *heightMap) : 
	heightMap_(heightMap),
	visible_(false), recalculateErrors_(false),
	leftPatch_(0), rightPatch_(0),
	topPatch_(0), bottomPatch_(0),
	visibilityIndex_(-1),
	dataSize_(0)
{
}

HeightMapVisibilityPatch::~HeightMapVisibilityPatch()
{
}

void HeightMapVisibilityPatch::setLocation(int x, int y,
	HeightMapVisibilityPatch *leftPatch, 
	HeightMapVisibilityPatch *rightPatch, 
	HeightMapVisibilityPatch *topPatch, 
	HeightMapVisibilityPatch *bottomPatch)
{
	int mapWidth = heightMap_->getMapWidth();
	int mapHeight = heightMap_->getMapHeight();

	DIALOG_ASSERT(x >= 0 && y >= 0 &&
		x < mapWidth && y < mapHeight);

	// Set location and neighbors
	x_ = x; y_ = y;
	leftPatch_ = leftPatch;
	rightPatch_ = rightPatch;
	topPatch_ = topPatch;
	bottomPatch_ = bottomPatch;

	// Set pointers to heightmap
	dataSize_ = (mapWidth + 1) * (mapHeight + 1);
	dataOffSet_ = (x + (y * (mapWidth + 1))) * 
		sizeof(GraphicalLandscapeMap::HeightData) / sizeof(float);

	calculateErrors();
}

float HeightMapVisibilityPatch::getHeight(int x, int y)
{
	int mapWidth = heightMap_->getMapWidth();

	GraphicalLandscapeMap *landscapeMap = (GraphicalLandscapeMap *) heightMap_->getGraphicalMap();
	return landscapeMap->getHeightData()[x + (y * (mapWidth + 1))].floatPosition[2];
}

/*
 x1y2*    xmy2~    x2y2*

 x1ym~    xmym~    x2ym~

 x1y1*    xmy1~    x2y1*

 The * values are passed in.  
 The ~ values are calculated from the * values.
*/
float HeightMapVisibilityPatch::calculateError(int x1, int x2, int y1, int y2,
	float x1y1, float x2y2, float x1y2, float x2y1)
{
	int xm = (x1 + x2) / 2;
	int ym = (y1 + y2) / 2;

	// Get the actual heights
	float xmymHeight = getHeight(xm, ym);
	float x1ymHeight = getHeight(x1, ym);
	float x2ymHeight = getHeight(x2, ym);
	float xmy2Height = getHeight(xm, y2);
	float xmy1Height = getHeight(xm, y1);

	// Get the heights that will be drawn
	float xmymApprox = (x1y1 + x2y2) / 2.0f;
	float x1ymApprox = (x1y1 + x1y2) / 2.0f;
	float x2ymApprox = (x2y1 + x2y2) / 2.0f;
	float xmy2Approx = (x1y2 + x2y2) / 2.0f;
	float xmy1Approx = (x1y1 + x2y1) / 2.0f;

	// Calculate the difference between the actual heights and the 
	// heights that will be drawn i.e. the error
	float xmymError = fabs(xmymApprox - xmymHeight);
	float x1ymError = fabs(x1ymApprox - x1ymHeight);
	float x2ymError = fabs(x2ymApprox - x2ymHeight);
	float xmy2Error = fabs(xmy2Approx - xmy2Height);
	float xmy1Error = fabs(xmy1Approx - xmy1Height);

	// The error is the maximum error of the above
	float totalError = 
		MAX(xmymError, 
		MAX(x1ymError, 
		MAX(x2ymError, 
		MAX(xmy2Error, xmy1Error))));

	// Recurse to get errors for sub-squares
	if (x1 - x2 > 2)
	{
		float childError1 = calculateError(x1, xm, y1, ym, x1y1, xmymApprox, x1ymApprox, xmy1Approx);
		float childError2 = calculateError(xm, x2, y1, ym, xmy1Approx, x2ymApprox, xmymApprox, x2y1);
		float childError3 = calculateError(x1, xm, ym, y2, x1ymApprox, xmy2Approx, x1y2, xmymApprox);
		float childError4 = calculateError(xm, x2, ym, y2, xmymApprox, x2y2, xmy2Approx, x2ymApprox);
		float childError = MAX(childError1, MAX(childError2, MAX(childError3, childError4)));
		totalError = MAX(childError, totalError);
	}

	return totalError;
}

float HeightMapVisibilityPatch::calculateError2(int x, int y, int width, float &minHeight, float &maxHeight)
{
	static float left[33], right[33];

	float topleft = getHeight(x, y + width);
	float botleft = getHeight(x, y);
	float topright = getHeight(x + width, y + width);
	float botright = getHeight(x + width, y);
	for (int b=0; b<=width; b++)
	{
		left[b] = botleft + (topleft - botleft) * float(b) / float(width);
		right[b] = botright + (topright - botright) * float(b) / float(width);

		if (minHeight > left[b]) minHeight = left[b];
		if (maxHeight < left[b]) maxHeight = left[b];
	}

	float maxError = 0.0f;
	for (int b=0; b<=width; b++)
	{
		for (int a=0; a<=width; a++)
		{
			float approxPosition = left[b] + (right[b] - left[b]) * float(a) / float(width);
			float actualPosition = getHeight(x + a, y + b);
			float error = fabs(approxPosition - actualPosition);
			maxError = MAX(maxError, error);
		}
	}
	return maxError;
}

void HeightMapVisibilityPatch::calculateErrors()
{
	maxHeight_ = 0.0f;
	minHeight_ = 100000.0f;

	for (int i=1; i<=5; i++)
	{
		float maxError = 0.0f;
		int skip = 1 << i;
		for (int b=0; b<32; b+=skip)
		{
			for (int a=0; a<32; a+=skip)
			{
				float currentError = calculateError2(x_ + a, y_ + b, skip, minHeight_, maxHeight_);
				maxError = MAX(maxError, currentError);
			}
		}

		indexErrors_[i-1] = maxError;
	}

	float heightRange = maxHeight_ - minHeight_;
	boundingSize_ = MAX(32.0f, heightRange) * 1.25f;
	position_ = Vector(float(x_ + 16), float(y_ + 16), 
		heightRange / 2.0f + minHeight_);
}

bool HeightMapVisibilityPatch::setVisible(float distance, float C)
{ 
	visible_ = true;

	visibilityIndex_ = 0;
	if (!OptionsDisplay::instance()->getNoLandLOD())
	{
		if (recalculateErrors_) 
		{
			calculateErrors();
			recalculateErrors_ = false;
		}

		float errorDistance = distance / C;
		if (errorDistance >= indexErrors_[4]) 
		{
			visibilityIndex_ = 5;
		}
		else if (errorDistance >= indexErrors_[3]) 
		{
			visibilityIndex_ = 4;
		}
		else if (errorDistance >= indexErrors_[2]) 
		{
			visibilityIndex_ = 3;
		}
		else if (errorDistance >= indexErrors_[1]) 
		{
			visibilityIndex_ = 2;
		}
		else if (errorDistance >= indexErrors_[0]) 
		{
			visibilityIndex_ = 1;
		}
	}

	return true;
}

void HeightMapVisibilityPatch::setNotVisible()
{
	visible_ = false;
}

void HeightMapVisibilityPatch::draw(MipMapPatchIndex &index, bool simple)
{
	GraphicalLandscapeMap *landscapeMap = (GraphicalLandscapeMap *) heightMap_->getGraphicalMap();
	float *heightMapData = &landscapeMap->getHeightData()->floatPosition[0];

	// Number triangles
	GLInfo::addNoTriangles(index.getSize() - 2);

	if (!OptionsDisplay::instance()->getNoGLDrawElements() &&
		GLStateExtension::hasDrawRangeElements())
	{
		// Map data to draw
		float *data = 0;
		if (landscapeMap->getBufferObject())
		{
			data = (float*) NULL + dataOffSet_;
		}
		else
		{
			data = &heightMapData[dataOffSet_];
		}

		// Vertices On
		glVertexPointer(3, GL_FLOAT, sizeof(GraphicalLandscapeMap::HeightData), data);

		if (!simple)
		{
			// Normals On
			glNormalPointer(GL_FLOAT, sizeof(GraphicalLandscapeMap::HeightData), data + 3);

			// Tex Coords
			if (GLStateExtension::hasMultiTex())
			{
				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glTexCoordPointer(2, GL_FLOAT, sizeof(GraphicalLandscapeMap::HeightData), data + 6);
				if (GLStateExtension::getTextureUnits() > 2)
				{
					glClientActiveTextureARB(GL_TEXTURE2_ARB);
					glTexCoordPointer(2, GL_FLOAT, sizeof(GraphicalLandscapeMap::HeightData), data + 8);
				}
			}
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, sizeof(GraphicalLandscapeMap::HeightData), data + 6);
		}

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
				float *data = &heightMapData[dataOffSet_] + 
					(sizeof(GraphicalLandscapeMap::HeightData) / 4 * index.getIndices()[i]);
				if (!simple)
				{
					glNormal3fv(data + 3);
					glTexCoord2fv(data + 6);
					if (GLStateExtension::hasMultiTex())
					{
						glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, data + 6);
						if (GLStateExtension::getTextureUnits() > 2)
						{
							glMultiTexCoord2fvARB(GL_TEXTURE2_ARB, data + 8);
						}
					}
				}

				glVertex3fv(data);
			}
		glEnd();
	}
}

void HeightMapVisibilityPatch::drawLODLevel(MipMapPatchIndex &index)
{
	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT, GL_FILL);

	GraphicalLandscapeMap *landscapeMap = (GraphicalLandscapeMap *) heightMap_->getGraphicalMap();
	float *heightMapData = &landscapeMap->getHeightData()->floatPosition[0];

	Vector red(1.0f, 0.0f, 0.0f);
	Vector yellow(1.0f, 1.0f, 0.0f);
	GLWFont::instance()->getGameFont()->drawBilboard(red, 1.0f, 3.0f, 
		position_[0], position_[1], position_[2], 
		S3D::formatStringBuffer("%i", visibilityIndex_));

	if ((MainCamera::instance()->getCamera().getLookAt() - position_).Magnitude() < 20.0f)
	{
		for (int i=0; i<index.getSize(); i++)
		{
			float *data = &heightMapData[dataOffSet_] + 
				(sizeof(GraphicalLandscapeMap::HeightData) / 4 * index.getIndices()[i]);

			GLWFont::instance()->getGameFont()->drawBilboard(yellow, 1.0f, 1.0f, 
				data[0], data[1], data[2] + i * 1.5f, 
				S3D::formatStringBuffer("%i", i));
		}
	}

	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT, GL_LINE);
}
