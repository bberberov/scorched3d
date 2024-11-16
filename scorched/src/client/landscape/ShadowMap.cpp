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

#include <math.h>
#include <string.h>
#include <GLEXT/GLStateExtension.h>
#include <landscape/ShadowMap.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>

ShadowMap::ShadowMap() : size_(256), sizeSq_(256 *256), shadowCount_(0)
{
	shadowBytes_ = new GLubyte[sizeSq_];
	memset(shadowBytes_, 255, sizeSq_);
	shadowTexture_.create(shadowBytes_, size_, size_, 1, 4, GL_LUMINANCE, false);
}

ShadowMap::~ShadowMap()
{
	delete [] shadowBytes_;
}

void ShadowMap::setTexture()
{
	shadowCount_ = 0;

	// Set the shadow map texture
	shadowTexture_.draw(true);

	if (!GLStateExtension::getNoTexSubImage())
	{
		// Update with the contents from the stored bytes
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
						0, 0, 
						size_, size_, 
						GL_LUMINANCE, GL_UNSIGNED_BYTE, 
						shadowBytes_);

		// Reset the stored bytes
		memset(shadowBytes_, 255, sizeSq_);

		// Debuging stripe
		//for (int i=0; i<size_; i++) shadowBytes_[i + i * size_] = 0;
	}
}

void ShadowMap::addSquare(float sx, float sy, float sw, float opacity)
{
	addShadow(sx, sy, sw, opacity, false);
}

void ShadowMap::addCircle(float sx, float sy, float sw, float opacity)
{
	addShadow(sx, sy, sw, opacity, true);
}

bool ShadowMap::shouldAddShadow()
{
	if (!GLStateExtension::hasMultiTex() ||
		GLStateExtension::getNoTexSubImage() ||
		OptionsDisplay::instance()->getNoShadows() ||
		GLStateExtension::hasHardwareShadows()) return false;
	return true;
}

void ShadowMap::addShadow(float mapx, float mapy, float mapw, float opacity, bool circle)
{
	if (!shouldAddShadow()) return;

	if (mapw > 10.0f) return; // Sanity check

	int mapWidth = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int mapHeight = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	float sx = mapx * size_ / float(mapWidth);
	float sy = mapy * size_ / float(mapHeight);
	float sw = mapw * size_ / float(mapWidth); // Make sure the size is scaled
	if (sx < 0 || sx >= size_ || sy < 0 || sy >= size_)
	{
		return;
	}

	shadowCount_++;
	const GLubyte minNum = 64;
	int decrement = int(opacity * 200.0f);
	float halfW = sw / 2.0f;

	float minX = sx - halfW;
	float minY = sy - halfW;
	float maxX = sx + halfW;
	float maxY = sy + halfW;

	minX = MAX(minX, 0.0f);
	minY = MAX(minY, 0.0f);
	maxX = MIN(maxX, size_ - 1.0f);
	maxY = MIN(maxY, size_ - 1.0f);

	int iSize = int(size_);
	int xStart = int(minX);
	int yStart = int(minY);
	int xWidth = int(maxX - minX);
	int yWidth = int(maxY - minY);
	int yInc = size_ - xWidth;
	if (xWidth <= 0 || yWidth <= 0) return;

	if (circle)
	{
		static float sintable[100];
		static bool sintablemade = false;
		if (!sintablemade)
		{
			sintablemade = true;
			float degMult = 0.0314f;
			for (int i=0; i<100; i++)
			{
				float deg = float(i) * degMult;
				sintable[i] = sinf(deg);
			}
		}
		GLubyte *start = &shadowBytes_[(yStart * iSize) + xStart];
		for (int y=0; y<yWidth; y++, start += yInc)
		{
			int deg = (y * 90) / yWidth;
			int realXSize = int(sintable[deg] * xWidth);
			int halfSize = (xWidth - realXSize) / 2;

			start+=halfSize;
			int x;
			for (x=0; x<realXSize; x++, start++)
			{
				if (*start > decrement + minNum) (*start) -= (GLubyte) (decrement);
				else (*start = minNum);
			}
			start+=xWidth - (halfSize + x);
		}
	}
	else
	{
		GLubyte *start = &shadowBytes_[(yStart * iSize) + xStart];
		for (int y=0; y<yWidth; y++, start += yInc)
		{
			for (int x=0; x<xWidth; x++, start++)
			{
				if (*start > decrement + minNum) (*start) -= (GLubyte) decrement;
				else (*start = minNum);
			}
		}
	}
}
