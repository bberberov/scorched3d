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

#include <vector>
#include <math.h>
#include <GLEXT/GLImageItterator.h>
#include <GLEXT/GLImageModifier.h>
#include <image/Image.h>
#include <image/ImageFactory.h>
#include <engine/ScorchedContext.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <lang/LangResource.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>

bool ImageModifier::findIntersection(HeightMap &hMap,
										Vector start,
										Vector end,
										float &dist,
										float stopDist)
{
	bool result = false;
	dist = 0.0f;
	Vector point = start;
	Vector direction = end - start;
	float &pt0 = point[0];
	float &pt1 = point[1];
	float &pt2 = point[2];
	int width = hMap.getMapWidth();
	int height = hMap.getMapHeight();

	// Calculate how many pixels to jump for each itteration
	if (fabsf(direction[0]) > fabsf(direction[1])) direction /= fabsf(direction[0]);
	else direction /= fabsf(direction[1]);

	while (pt0 >= 0.0f && pt1 >= 0.0f &&
		pt0 <= width && pt1 <= height)
	{
		float height = hMap.getHeight(int(point[0]), int(point[1])).asFloat() - 0.1f;
		float rayHeight = height - pt2;
		if (rayHeight > 0.0f)
		{
			if (rayHeight > dist) dist = rayHeight;
			result = true;
			if (dist > stopDist) return result;
		}

		point += direction;
	}

	return result;
}

void ImageModifier::tileBitmap(Image &src, Image &dest)
{
	DIALOG_ASSERT(dest.getComponents() == 3);

	GLubyte *destBytes = dest.getBits();
	for (int j=0; j<dest.getHeight(); j++)
	{
		for (int i=0; i<dest.getWidth();i++, destBytes+=3)
		{
			int srcX = i % src.getWidth();
			int srcY = j % src.getHeight();

			GLubyte *srcBytes = src.getBits() + (src.getComponents() * srcX) +
				(src.getComponents() * src.getWidth() * srcY);

			destBytes[0] = srcBytes[0];
			if (src.getComponents() == 1)
			{
				destBytes[1] = srcBytes[0];
				destBytes[2] = srcBytes[0];
			}
			else
			{
				destBytes[1] = srcBytes[1];
				destBytes[2] = srcBytes[2];
			}
		}
	}
}

void ImageModifier::addLightMapToBitmap(Image &destBitmap, 
	HeightMap &hMap,
	Vector &sunPos,
	Vector &ambience,
	Vector &diffuse,
	ProgressCounter *counter)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	const float softShadow = 3.0f;
	const int lightMapWidth = 256; // Resolution of the light map

	if (counter) counter->setNewOp(LANG_RESOURCE("LIGHT_MAP", "Light Map"));

	// Itterate the dest bitmap pixels
	GLfloat *bitmap = new GLfloat[lightMapWidth * lightMapWidth * 3];
	GLfloat *bitmapBits = bitmap;
	int y;
	for (y=0; y<lightMapWidth; y++)
	{
		if (counter) counter->setNewPercentage(100.0f * float(y) / float(lightMapWidth));

		for (int x=0; x<lightMapWidth; x++)
		{
			float dx = float(x)/float(lightMapWidth)*float(hMap.getMapWidth());
			float dy = float(y)/float(lightMapWidth)*float(hMap.getMapHeight());
			float dz = hMap.getInterpHeight(
				fixed::fromFloat(dx), fixed::fromFloat(dy)).asFloat();
			
			Vector testPosition(dx, dy, dz);
			FixedVector fixedTestNormal;
			hMap.getInterpNormal(
				fixed::fromFloat(dx), fixed::fromFloat(dy), fixedTestNormal);
			Vector testNormal = fixedTestNormal.asVector();
			Vector sunDirection = (sunPos - testPosition).Normalize();

			// Calculate light based on whether obejcts in path
			float diffuseLightMult = 
				(((testNormal.dotP(sunDirection)) / 2.0f) + 0.5f);
			float dist = 0.0f;
			if (findIntersection(hMap, 
				testPosition, sunPos, dist, softShadow))
			{
				// An object is in the path
				if (dist < softShadow)
				{
					// The object is only just in the path
					// Create soft shadow
					diffuseLightMult *= 1.0f - (dist / softShadow);
				}
				else
				{
					// Totaly in path, dark shadow
					diffuseLightMult = 0.0f;
				}
			}

			Vector diffuseLight = diffuse * diffuseLightMult;
			Vector ambientLight = ambience;
			Vector lightColor = diffuseLight + ambientLight;
			lightColor[0] = MIN(1.0f, lightColor[0]);
			lightColor[1] = MIN(1.0f, lightColor[1]);
			lightColor[2] = MIN(1.0f, lightColor[2]);

			bitmapBits[0] = lightColor[0];
			bitmapBits[1] = lightColor[1];
			bitmapBits[2] = lightColor[2];
			bitmapBits +=3;
		}
	}

	GLfloat *copyDest = new GLfloat[destBitmap.getWidth() * destBitmap.getHeight() * 3];
	gluScaleImage(
		GL_RGB, 
		lightMapWidth, lightMapWidth, 
		GL_FLOAT, bitmap,
		destBitmap.getWidth(), destBitmap.getHeight(), 
		GL_FLOAT, copyDest);
	
	GLfloat *srcBits = copyDest;
	GLubyte *destBits = destBitmap.getBits();
	for (y=0; y<destBitmap.getHeight(); y++)
	{
		for (int x=0; x<destBitmap.getWidth(); x++)
		{
			destBits[0] = GLubyte(MIN(float(destBits[0]) * (srcBits[0] * 1.2f), 255.0f));
			destBits[1] = GLubyte(MIN(float(destBits[1]) * (srcBits[1] * 1.2f), 255.0f));
			destBits[2] = GLubyte(MIN(float(destBits[2]) * (srcBits[2] * 1.2f), 255.0f));

			srcBits += 3;
			destBits += 3;
		}
	}

	delete [] copyDest;
	delete [] bitmap;
}

static void blendPixels(GLubyte *destBits, GLubyte *sourceBits, int sourceComponents, float blendAmount)
{
	if (sourceComponents == 1)
	{
		destBits[0] += (GLubyte) ((float) sourceBits[0] * blendAmount);
		destBits[1] += (GLubyte) ((float) sourceBits[0] * blendAmount);
		destBits[2] += (GLubyte) ((float) sourceBits[0] * blendAmount);
	}
	else
	{
		destBits[0] += (GLubyte) ((float) sourceBits[0] * blendAmount);
		destBits[1] += (GLubyte) ((float) sourceBits[1] * blendAmount);
		destBits[2] += (GLubyte) ((float) sourceBits[2] * blendAmount);
	}
}

void ImageModifier::addHeightToBitmap(HeightMap &hMap,
										 Image &destBitmap, 
										 Image &slopeBitmap,
										 Image &shoreBitmap,
										 Image **origHeightBitmaps,
										 int numberSources,
										 int destBitmapScaleSize,
										 ProgressCounter *counter)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	const float maxHeight = 30.0f; // Last texture ends at height 30
	const float blendHeightFactor = 0.4f; // Ends blend when 40% into height band
	const float blendNormalSlopeStart = 0.8f; // Starts blending slope at .80
	const float blendNormalSlopeLength = 0.3f; // Blends when 30% more slope
	const float blendNormalShoreStart = 0.8f; // Starts the sand
	const float blendNormalShoreLength = 0.1f; // Amount of sand
	const float noiseMax = 0.4f;

	float hMapMaxHeight = 0;
	for (int ma=0; ma<hMap.getMapWidth(); ma++)
	{
		for (int mb=0;mb<hMap.getMapHeight(); mb++)
		{
			float height = hMap.getHeight(ma, mb).asFloat();
			if (height > hMapMaxHeight) hMapMaxHeight = height;
		}
	}

	// Create new bitmaps with the bitmap scaled to the correct size
	Image **heightBitmaps = new Image*[numberSources];
	ImageItterator ** bitmapItors = new ImageItterator*[numberSources+2];
	float bitmapScale = float(destBitmap.getWidth()) / float(destBitmapScaleSize);

	// Create a bitmap iterator for each bitmap
	// Create a bitmap correctly scaled to the scene
	int i;
	for (i=0; i<numberSources; i++)
	{
		if (bitmapScale != 1.0f)
		{
			// Create the newly scaled bitmaps
			heightBitmaps[i] = new Image(origHeightBitmaps[i]->createResize(
				int(bitmapScale * origHeightBitmaps[i]->getWidth()),
				int(bitmapScale * origHeightBitmaps[i]->getHeight())));
		}
		else
		{
			heightBitmaps[i] = origHeightBitmaps[i];
		}

		// Create iterator
		bitmapItors[i] = new ImageItterator(
			*heightBitmaps[i], 
			destBitmap.getWidth(), 
			destBitmap.getHeight(), 
			ImageItterator::wrap);
	}
	// Add shore and slopt itterators
	bitmapItors[numberSources] = 
		new ImageItterator(
			slopeBitmap, 
			destBitmap.getWidth(), 
			destBitmap.getHeight(), 
			ImageItterator::wrap);
	bitmapItors[numberSources + 1] = 
		new ImageItterator(
			shoreBitmap, 
			destBitmap.getWidth(), 
			destBitmap.getHeight(), 
			ImageItterator::wrap);
	
	GLfloat hdx = (GLfloat) hMap.getMapWidth() / (GLfloat) destBitmap.getWidth();
	GLfloat hdy = (GLfloat) hMap.getMapHeight() / (GLfloat) destBitmap.getHeight();

	GLubyte *destBits = destBitmap.getBits();

	GLfloat hy = 0.0f;
	for (int by=0; by<destBitmap.getHeight(); by++, hy+=hdy)
	{
		if (counter) counter->setNewPercentage((100.0f * float (by)) / float(destBitmap.getHeight()));

		GLfloat hx = 0.0f;
		for (int bx=0; bx<destBitmap.getWidth(); bx++, destBits+=3, hx+=hdx)
		{
			static FixedVector fixedNormal;
			hMap.getInterpNormal(fixed::fromFloat(hx), fixed::fromFloat(hy), fixedNormal);
			Vector &normal = fixedNormal.asVector();
			float height = hMap.getInterpHeight(fixed::fromFloat(hx), fixed::fromFloat(hy)).asFloat();
			float offSetHeight = hMap.getInterpHeight(
				fixed::fromFloat((float)hMap.getMapWidth() - hx), 
				fixed::fromFloat((float)hMap.getMapHeight() - hy)).asFloat();
			height *= (1.0f - (noiseMax/2.0f)) + ((offSetHeight*noiseMax)/hMapMaxHeight);

			// Find the index of the current texture by deviding the height into strips
			float heightPer = (height / maxHeight) * (float) numberSources;
			int heightIndex = (int) heightPer;
			if (heightIndex >= numberSources) 
			{
				heightIndex = numberSources - 1;
			}

			// Check if we are in a blending transition phase
			float blendFirstAmount = 1.0f;
			float blendSecondAmount = 0.0f;
			if (heightIndex < numberSources - 1)
			{
				float remainderIndex = heightPer - heightIndex;
				if (remainderIndex > blendHeightFactor)
				{
					// We need to do some blending, figure how much
					remainderIndex -= blendHeightFactor;
					blendSecondAmount = remainderIndex / (1.0f - blendHeightFactor);
					blendFirstAmount = 1.0f - blendSecondAmount;
				}
			}

			// Check to see if we need to blend in the side texture
			float blendSideAmount = 0.0f;
			float blendShoreAmount = 0.0f;
			if (normal[2] < blendNormalSlopeStart)
			{
				if (normal[2] < blendNormalSlopeStart - blendNormalSlopeLength)
				{
					// Only use the side texture
					blendSideAmount = 1.0f;
					blendFirstAmount = 0.0f;
					blendSecondAmount = 0.0f;
				}
				else
				{
					// Blend in the side texture
					float remainderIndex = normal[2] - (blendNormalSlopeStart - blendNormalSlopeLength);
					remainderIndex /= blendNormalSlopeLength;
				
					blendSideAmount = (1.0f - remainderIndex);
					blendFirstAmount *= remainderIndex;
					blendSecondAmount *= remainderIndex;
				}
			} 
			else if (normal[2] > blendNormalShoreStart && 
				height > 3.5f && height < 5.5f)
			{
				if (normal[2] > blendNormalShoreStart + blendNormalShoreLength)
				{
					// Only use the side texture
					blendShoreAmount = 1.0f;
					blendFirstAmount = 0.0f;
					blendSecondAmount = 0.0f;
				}
				else
				{
					// Blend in the side texture
					float remainderIndex = normal[2] - blendNormalSlopeStart;
					remainderIndex /= blendNormalSlopeLength;
				
					blendShoreAmount = (1.0f - remainderIndex);
					blendFirstAmount *= remainderIndex;
					blendSecondAmount *= remainderIndex;
				}
			}

			destBits[0] = destBits[1] = destBits[2] = 0;

			// Add first height component
			GLubyte *sourceBits1 = bitmapItors[heightIndex]->getPos();
			blendPixels(destBits, sourceBits1, bitmapItors[heightIndex]->getComponents(), blendFirstAmount);

			if (blendSecondAmount > 0.0f)
			{
				// Add second height component (if blending)
				GLubyte *sourceBits2 = bitmapItors[heightIndex + 1]->getPos();
				blendPixels(destBits, sourceBits2, bitmapItors[heightIndex + 1]->getComponents(), blendSecondAmount);
			}

			if (blendSideAmount > 0.0f)
			{
				// Add side component (if blending normals)
				GLubyte *sourceBits3 = bitmapItors[numberSources]->getPos();
				blendPixels(destBits, sourceBits3, bitmapItors[numberSources]->getComponents(), blendSideAmount);
			}

			if (blendShoreAmount > 0.0f)
			{
				// Add side component (if blending normals)
				GLubyte *sourceBits4 = bitmapItors[numberSources + 1]->getPos();
				blendPixels(destBits, sourceBits4, bitmapItors[numberSources + 1]->getComponents(), blendShoreAmount);
			}

			for (i=0; i<numberSources+2; i++) bitmapItors[i]->incX();
		}

		for (i=0; i<numberSources+2; i++) bitmapItors[i]->incY();
	}

	// Cleanup iterator and extra bitmaps	
	for (i=0; i<numberSources+2; i++)
	{
		delete bitmapItors[i];
	}
	delete [] bitmapItors;
	for (i=0; i<numberSources; i++)
	{
		if (bitmapScale != 1.0f)
		{
			delete heightBitmaps[i];
		}
	}
	delete [] heightBitmaps;
}

void ImageModifier::redBitmap(Image &destBitmap)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	unsigned char *destBits = destBitmap.getBits();
	for (int y=0; y<destBitmap.getHeight(); y++)
	{
		for (int x=0; x<destBitmap.getWidth(); x++, destBits += 4)
		{
			destBits[0] = 255;
			destBits[1] = 0;
			destBits[2] = 0;
			destBits[3] = 0;
		}
	}
}

void ImageModifier::addTexturesToBitmap(
		Image &destBitmap,
		Image &slopeBitmap,
		Image &shoreBitmap,
		Image **heightBitmaps,
		int numberSources)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	std::vector<Image *> sources;
	for (int i=0; i<numberSources; i++)
	{
		sources.push_back(heightBitmaps[i]);
	}
	sources.push_back(&slopeBitmap);
	sources.push_back(&shoreBitmap);

	int currentCount = (int) sources.size();
	for (int i=currentCount; i<9; i++)
	{
		sources.push_back(&shoreBitmap);
	}
	
	unsigned char *destBits = destBitmap.getBits();
	for (int y=0; y<destBitmap.getHeight(); y++)
	{
		for (int x=0; x<destBitmap.getWidth(); x++, destBits += 3)
		{
			int texx = x / (destBitmap.getWidth() / 3);
			int texy = y / (destBitmap.getHeight() / 3);
			texx = MIN(2, texx);
			texy = MIN(2, texy);

			Image *src = sources[texx + texy * 3];
			int srcx = x % src->getWidth();
			int srcy = y % src->getHeight();
			
			unsigned char *srcBits = &src->getBits()[srcx * 3 + srcy * src->getWidth() * 3];
			destBits[0] = srcBits[0];
			destBits[1] = srcBits[1];
			destBits[2] = srcBits[2];
		}
	}
}

void ImageModifier::removeWaterFromBitmap(HeightMap &hMap,
							Image &srcBitmap,
							Image &destBitmap,
							Image &alphaBitmap,
							float waterHeight)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 4);
	DIALOG_ASSERT(srcBitmap.getWidth() == destBitmap.getWidth() &&
		srcBitmap.getWidth() == alphaBitmap.getWidth());
	DIALOG_ASSERT(srcBitmap.getHeight() == destBitmap.getHeight() &&
		srcBitmap.getHeight() == alphaBitmap.getHeight());

	GLubyte *destBits = destBitmap.getBits();
	GLubyte *srcBits = srcBitmap.getBits();
	GLubyte *alphaBits = alphaBitmap.getBits();

	GLfloat hdx = (GLfloat) hMap.getMapWidth() / (GLfloat) destBitmap.getWidth();
	GLfloat hdy = (GLfloat) hMap.getMapHeight() / (GLfloat) destBitmap.getHeight();

	GLfloat hy = 0.0f;
	for (int y=0; y<srcBitmap.getHeight(); y++, hy+=hdy)
	{
		GLfloat hx = 0.0f;
		for (int x=0; x<srcBitmap.getWidth(); x++, hx+=hdx, 
			destBits+=destBitmap.getComponents(), srcBits+=srcBitmap.getComponents(), alphaBits+=alphaBitmap.getComponents())
		{
			GLubyte alpha = 255 - alphaBits[0];
			if (alpha > 0)
			{
				float height = hMap.getInterpHeight(
					fixed::fromFloat(hx), fixed::fromFloat(hy)).asFloat();
				if (height > waterHeight - 0.3)
				{
					alpha = 128;
					if (height > waterHeight)
					{
						alpha = 255;
					}
				}
				else alpha = 0;
			}

			destBits[0] = srcBits[0];
			if (srcBitmap.getComponents() == 1)
			{
				destBits[1] = srcBits[0];
				destBits[2] = srcBits[0];
			}
			else
			{
				destBits[1] = srcBits[1];
				destBits[2] = srcBits[2];
			}
			destBits[3] = alpha;
		}
	}
}

void ImageModifier::addWaterToBitmap(HeightMap &hMap,
										Image &destBitmap,
										Image &waterBitmap,
										float waterHeight)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	const float waterPercentage = 0.75f;
	const float oneMinusPercentage = 1.0f - waterPercentage;

	ImageItterator bitmapItor(waterBitmap,
								destBitmap.getWidth(), 
								destBitmap.getHeight(), 
								ImageItterator::wrap);

	GLfloat hdx = (GLfloat) hMap.getMapWidth() / (GLfloat) destBitmap.getWidth();
	GLfloat hdy = (GLfloat) hMap.getMapHeight() / (GLfloat) destBitmap.getHeight();

	GLubyte *destBits = destBitmap.getBits();

	GLfloat hy = 0.0f;
	for (int by=0; by<destBitmap.getHeight(); by++, hy+=hdy, bitmapItor.incY())
	{
		GLfloat hx = 0.0f;
		for (int bx=0; bx<destBitmap.getWidth(); bx++, destBits+=destBitmap.getComponents(), hx+=hdx, bitmapItor.incX())
		{
			float height = hMap.getInterpHeight(
				fixed::fromFloat(hx), fixed::fromFloat(hy)).asFloat();

			if (height <= waterHeight)
			{
				if (height <= waterHeight - 0.3)
				{
					GLubyte *sourceBits = bitmapItor.getPos();

					destBits[0] = GLubyte(
						(waterPercentage * float(sourceBits[0])) + 
						(oneMinusPercentage * float(destBits[0])));
					destBits[1] = GLubyte(
						(waterPercentage * float(sourceBits[1])) + 
						(oneMinusPercentage * float(destBits[1])));
					destBits[2] = GLubyte(
						(waterPercentage * float(sourceBits[2])) + 
						(oneMinusPercentage * float(destBits[2])));
				}
				else
				{
					destBits[0] = 200;
					destBits[1] = 200;
					destBits[2] = 200;
				}
			}
		}
	}
}

Image ImageModifier::makeArenaBitmap()
{
	Image handle(128, 128, 4, 0);

	int arenaX = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaHeight();
	int landscapeWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int landscapeHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	int lx = arenaX * handle.getWidth() / landscapeWidth;
	int ly = arenaY * handle.getHeight() / landscapeHeight;
	int lw = lx + (arenaWidth * handle.getWidth() / landscapeWidth);
	int lh = ly + (arenaHeight * handle.getHeight() / landscapeHeight);

	unsigned char *bits = handle.getBits();
	for (int y=0; y<handle.getHeight(); y++)
	{
		for (int x=0; x<handle.getWidth(); x++, bits+=4)
		{
			if (x >= lx && x < lw &&
				y >= ly && y < lh)
			{
				bits[3] = 0;
			}
			else 
			{
				bits[3] = 255;
			}
		}
	}

	return handle;
}

Image ImageModifier::makeArenaSurroundBitmap()
{
	Image handle(128, 128, 4, 0);

	unsigned char *bits = handle.getBits();
	for (int y=0; y<handle.getHeight(); y++)
	{
		for (int x=0; x<handle.getWidth(); x++, bits+=4)
		{
			bits[3] = 255;
		}
	}

	return handle;
}

void ImageModifier::addBorderToBitmap(Image &destBitmap,
	int borderSize,
	float colors[3])
{
	int arenaX = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaHeight();
	int landscapeWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int landscapeHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	int borderX = int(float(arenaX) / float(landscapeWidth) * float(destBitmap.getWidth()));
	int borderY = int(float(arenaY) / float(landscapeHeight) * float(destBitmap.getHeight()));
	int borderWidth = int(float(arenaWidth) / float(landscapeWidth) * float(destBitmap.getWidth()));
	int borderHeight = int(float(arenaHeight) / float(landscapeHeight) * float(destBitmap.getHeight()));
	if (borderWidth + borderX >= destBitmap.getWidth()) borderWidth = destBitmap.getWidth() - borderX - 1;
	if (borderHeight + borderY >= destBitmap.getHeight()) borderHeight = destBitmap.getHeight() - borderY - 1;

	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	for (int x=borderX; x<=borderX+borderWidth; x++)
	{
		for (int i=0; i<borderSize; i++)
		{
			int pos = (x * 3) + ((borderY + i) * destBitmap.getWidth() * 3);
			GLubyte *destBits = destBitmap.getBitsOffset(pos);
			destBits[0] = GLubyte(colors[0] * 255.0f);
			destBits[1] = GLubyte(colors[1] * 255.0f);
			destBits[2] = GLubyte(colors[2] * 255.0f);

			pos = (x * 3) + ((borderY + borderHeight - i) * destBitmap.getWidth() * 3);
			destBits = destBitmap.getBitsOffset(pos);
			destBits[0] = GLubyte(colors[0] * 255.0f);
			destBits[1] = GLubyte(colors[1] * 255.0f);
			destBits[2] = GLubyte(colors[2] * 255.0f);
		}
	}

	for (int y=borderY; y<=borderY+borderHeight; y++)
	{
		for (int i=0; i<borderSize; i++)
		{
			int pos = ((borderX + i) * 3) + (y * destBitmap.getWidth() * 3);
			GLubyte *destBits = destBitmap.getBitsOffset(pos);
			destBits[0] = GLubyte(colors[0] * 255.0f);
			destBits[1] = GLubyte(colors[1] * 255.0f);
			destBits[2] = GLubyte(colors[2] * 255.0f);

			pos = ((borderX + borderWidth - i) * 3) + (y * destBitmap.getWidth() * 3);
			destBits = destBitmap.getBitsOffset(pos);
			destBits[0] = GLubyte(colors[0] * 255.0f);
			destBits[1] = GLubyte(colors[1] * 255.0f);
			destBits[2] = GLubyte(colors[2] * 255.0f);
		}
	}
}

void ImageModifier::makeBitmapTransparent(Image &output,
		Image &input,
		Image &mask)
{
	DIALOG_ASSERT(output.getComponents() == 4);
	DIALOG_ASSERT(input.getComponents() == 3 || input.getComponents() == 1);
	DIALOG_ASSERT(mask.getComponents() == 4);
	DIALOG_ASSERT(output.getWidth() == input.getWidth());
	DIALOG_ASSERT(output.getWidth() == mask.getWidth());
	DIALOG_ASSERT(output.getHeight() == input.getHeight());
	DIALOG_ASSERT(output.getHeight() == mask.getHeight());

	GLubyte *outputBits = output.getBits();
	GLubyte *maskBits = mask.getBits();
	GLubyte *inputBits = input.getBits();

	for (int i=0; i<output.getWidth() * output.getHeight(); i++)
	{
		if (input.getComponents() == 1)
		{
			outputBits[0] = inputBits[0];
			outputBits[1] = inputBits[0];
			outputBits[2] = inputBits[0];
			outputBits[3] = maskBits[3];
		}
		else
		{
			outputBits[0] = inputBits[0];
			outputBits[1] = inputBits[1];
			outputBits[2] = inputBits[2];
			outputBits[3] = maskBits[3];
		}

		inputBits += input.getComponents();
		outputBits += 4;
		maskBits += 4;
	}
}

void ImageModifier::addCircleToLandscape(
	ScorchedContext &context,
	float sx, float sy, float sw, float opacity)
{
	float shadowMultWidth = (float) Landscape::instance()->getMainMap().getWidth() / 
		context.getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	float shadowMultHeight = (float) Landscape::instance()->getMainMap().getHeight() / 
		context.getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	addCircle(Landscape::instance()->getMainMap(),
		sx * shadowMultWidth, sy * shadowMultHeight, 
		sw * shadowMultWidth, opacity);
}

void ImageModifier::addCircle(Image &destBitmap, 
								 float sx, float sy, float sw, float opacity)
{
	float halfW = sw / 2.0f;

	float minX = sx - halfW;
	float minY = sy - halfW;
	float maxX = sx + halfW;
	float maxY = sy + halfW;
	/*minX /= 2.0f;
	minY /= 2.0f;
	maxX /= 2.0f;
	maxY /= 2.0f;*/

	minX = MAX(minX, 0.0f);
	minY = MAX(minY, 0.0f);
	maxX = MIN(maxX, destBitmap.getWidth() - 1.0f);
	maxY = MIN(maxY, destBitmap.getHeight() - 1.0f);

	int xStart = int(minX);
	int yStart = int(minY);
	int xWidth = int(maxX - minX);
	int yWidth = int(maxY - minY);
	int yInc = (destBitmap.getWidth() - xWidth) * 3;

	if (xWidth <= 0 || yWidth <= 0) return;
	double degMult = (1 / double(yWidth)) * 3.14;

	GLubyte *start = &destBitmap.getBits()[(yStart * destBitmap.getWidth() * 3) + xStart * 3];
	for (int y=0; y<yWidth; y++, start += yInc)
	{
		double deg = double(y) * degMult;
		int realXSize = int(sin(deg) * double(xWidth));
		int halfSize = (xWidth - realXSize) / 2;

		start+=halfSize * 3;
		int x;
		for (x=0; x<realXSize; x++, start+=3)
		{
			start[0] = start[0] / 2;
			start[1] = start[1] / 2;
			start[2] = start[2] / 2;
		}
		start+=(xWidth - (halfSize + x)) * 3;
	}
}


void ImageModifier::addBitmapToLandscape(
	ScorchedContext &context,
	Image &srcBitmap,
	float sx, float sy, float scalex, float scaley, 
	bool commit)
{
	float shadowMultWidth = (float) Landscape::instance()->getMainMap().getWidth() / 
		context.getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	float shadowMultHeight = (float) Landscape::instance()->getMainMap().getHeight() / 
		context.getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	addBitmap(
		Landscape::instance()->getMainMap(),
		srcBitmap,
		sx * shadowMultWidth, 
		sy * shadowMultHeight,
		shadowMultWidth * scalex,
		shadowMultHeight * scaley,
		commit);
}

void ImageModifier::addBitmap(Image &destBitmap,
	Image &srcBitmap,
	float sx, float sy, float scalex, float scaley, 
	bool commit)
{
	DIALOG_ASSERT(destBitmap.getComponents() == 3);

	int srcScaleWidth = int(float(srcBitmap.getWidth()) * scalex);
	int srcScaleHeight = int(float(srcBitmap.getHeight()) * scaley);

	float minX = sx - srcScaleWidth / 2;
	float minY = sy - srcScaleHeight / 2;
	float maxX = sx + srcScaleWidth / 2;
	float maxY = sy + srcScaleHeight / 2;

	minX = MAX(minX, 0.0f);
	minY = MAX(minY, 0.0f);
	maxX = MIN(maxX, destBitmap.getWidth() - 1.0f);
	maxY = MIN(maxY, destBitmap.getHeight() - 1.0f);

	int xStart = int(minX);
	int yStart = int(minY);
	int xWidth = int(maxX - minX);
	int yWidth = int(maxY - minY);

	if (xWidth <= 0 || yWidth <= 0) return;

	int yDestInc = (destBitmap.getWidth() * 3);

	GLubyte *dest = &destBitmap.getBits()[
		(yStart * destBitmap.getWidth() * 3) + xStart * 3];
	for (int y=0; y<yWidth; y++, dest += yDestInc)
	{
		GLubyte *tmpDest = dest;
		for (int x=0; x<xWidth; x++)
		{
			int srcX = int(float(x) / scalex);
			srcX = MIN(srcX, srcBitmap.getWidth());
			int srcY = int(float(y) / scaley);
			srcY = MIN(srcY, srcBitmap.getHeight());

			GLubyte *tmpSrc = srcBitmap.getBits() +
				srcX * srcBitmap.getComponents() +
				srcY * srcBitmap.getComponents() * srcBitmap.getWidth();

			float alpha = 1.0f;
			float invAlpha = 0.0f;
			if (srcBitmap.getComponents() == 4)
			{
				alpha = float(tmpSrc[3]) / 255.0f;
				invAlpha = 1.0f - alpha;
			}

			tmpDest[0] = GLubyte(float(tmpSrc[0]) * alpha + float(tmpDest[0]) * invAlpha);
			if (srcBitmap.getComponents() == 1)
			{
				tmpDest[1] = GLubyte(float(tmpSrc[0]) * alpha + float(tmpDest[1]) * invAlpha);
				tmpDest[2] = GLubyte(float(tmpSrc[0]) * alpha + float(tmpDest[2]) * invAlpha);
			}
			else
			{
				tmpDest[1] = GLubyte(float(tmpSrc[1]) * alpha + float(tmpDest[1]) * invAlpha);
				tmpDest[2] = GLubyte(float(tmpSrc[2]) * alpha + float(tmpDest[2]) * invAlpha);
			}

			tmpDest += 3;
		}
	}

	if (commit)
	{
		int landscapeWidth = Landscape::instance()->getMainMap().getWidth();
		int width = 3 * landscapeWidth;
		width   = (width + 3) & ~3;	

		GLubyte *bytes = 
			Landscape::instance()->getMainMap().getBits() + ((width * yStart) + xStart * 3);

		GLState currentState(GLState::TEXTURE_ON);
		Landscape::instance()->getMainTexture().draw(true);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, landscapeWidth);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
						xStart, yStart, 
						xWidth, yWidth, 
						GL_RGB, GL_UNSIGNED_BYTE, 
						bytes);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
}

void ImageModifier::scalePlanBitmap(Image &destBitmap,
	Image &srcIncBitmap,
	int landscapeX, int landscapeY)
{
	int maxSize = MAX(landscapeX, landscapeY);
	float xScale = landscapeX / float(maxSize);
	float yScale = landscapeY / float(maxSize);

	int newX = int(float(destBitmap.getWidth()) / xScale);
	int newY = int(float(destBitmap.getHeight()) / yScale);
	int offsetX = (newX - destBitmap.getWidth()) / 2;
	int offsetY = (newY - destBitmap.getHeight()) / 2;

	Image srcBitmap = 
		srcIncBitmap.createResize(newX, newY);

	GLubyte *dest = destBitmap.getBits();
	for (int y=0; y<destBitmap.getHeight(); y++)
	{
		for (int x=0; x<destBitmap.getWidth(); x++, dest+=destBitmap.getComponents())
		{
			int srcX = MIN(x + offsetX, srcBitmap.getWidth() - 1);
			int srcY = MIN(y + offsetY, srcBitmap.getHeight() - 1);
			GLubyte *src = srcBitmap.getBits() +
				srcX * srcBitmap.getComponents() +
				srcY * srcBitmap.getComponents() * srcBitmap.getWidth();			

			dest[0] = src[0];
			dest[1] = src[1];
			dest[2] = src[2];
		}
	}
}
