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

#include <water/WaterWaves.h>
#include <water/WaterWaveDistance.h>
#include <water/Water2Patches.h>
#include <engine/Simulator.h>
#include <landscapemap/LandscapeMaps.h>
#include <lang/LangResource.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>
#include <image/ImageFactory.h>
#include <math.h>

WaterWaves::WaterWaves() : 
	totalTime_(0.0f), 
	wavesColor_(1.0f, 1.0f, 1.0f)
{

}

WaterWaves::~WaterWaves()
{
}

void WaterWaves::generateWaves(float waterHeight, ProgressCounter *counter)
{
	paths1_.clear();
	paths2_.clear();

	//if (OptionsDisplay::instance()->getNoWaves()) return;
	if (!OptionsDisplay::instance()->getDrawWater()) return;

	int mapWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeWidth();
	int mapHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeHeight();

	// Wave points
	WaterWaveContext context;
	context.pointCount = 0;
	context.removedCount = 0;
	context.pointsMult = 1;
	context.mapWidth = mapWidth;
	context.mapHeight = mapHeight;
	context.pointsWidth = mapWidth / context.pointsMult;
	context.pointsHeight = mapHeight / context.pointsMult;
	context.wavePoints = new bool[context.pointsWidth * context.pointsHeight];
	memset(context.wavePoints, 0, 
		context.pointsWidth * context.pointsHeight * sizeof(bool));

	// Find all of the points that are equal to a certain height (the water height)
	if (counter) counter->setNewOp(LANG_RESOURCE("CREATING_BREAKERS_1", "Creating Breakers 1"));
	findPoints(&context, waterHeight, counter);

	// Find the list of points that are next to eachother
	if (counter) counter->setNewOp(LANG_RESOURCE("CREATING_BREAKERS_2", "Creating Breakers 2"));
	while (findNextPath(&context, waterHeight, counter)) {}

	Image waves1 = ImageFactory::loadAlphaImage(
		S3D::eModLocation, "data/textures/waves.bmp");
	Image waves2 = ImageFactory::loadAlphaImage(
		S3D::eModLocation, "data/textures/waves2.bmp");
	wavesTexture1_.create(waves1);
	wavesTexture2_.create(waves2);

	delete [] context.wavePoints;
}

void WaterWaves::findPoints(WaterWaveContext *context,
	float waterHeight, ProgressCounter *counter)
{
	for (int y=context->pointsMult; 
		y<context->mapHeight - context->pointsMult; 
		y+=context->pointsMult)
	{
		if (counter) counter->setNewPercentage(float(y)/
			float(context->pointsHeight)*100.0f);
		for (int x=context->pointsMult; 
			x<context->mapWidth - context->pointsMult; 
			x+=context->pointsMult)
		{
			float height =
				ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y).asFloat();
			if (height > waterHeight - 4.0f && height < waterHeight)
			{
				float height2=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x+context->pointsMult, y).asFloat();
				float height3=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x-context->pointsMult, y).asFloat();
				float height4=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y+context->pointsMult).asFloat();
				float height5=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y-context->pointsMult).asFloat();

				if (height2 > waterHeight || height3 > waterHeight ||
					height4 > waterHeight || height5 > waterHeight)
				{
					{
						int a = x / context->pointsMult;
						int b = y / context->pointsMult;
						bool &pts = context->wavePoints[
							a + b * context->pointsWidth];
						if (!pts)
						{
							context->pointCount ++;
							pts = true;
						}
					}
				}
			}
		}
	}
}

bool WaterWaves::findNextPath(WaterWaveContext *context,
	float waterHeight, ProgressCounter *counter)
{
	std::vector<Vector> points;

	for (int y=1; y<context->pointsHeight; y++)
	{
		for (int x=1; x<context->pointsWidth; x++)
		{
			if (context->wavePoints[x + y * context->pointsWidth]) 
			{
				findPath(context, points, x, y, counter);
				constructLines(context, waterHeight, points);
				return true;			
			}
		}
	}

	return false;
}

void WaterWaves::findPath(WaterWaveContext *context,
	std::vector<Vector> &points, 
	int x, int y, ProgressCounter *counter)
{
	if (x < 0 || x >= context->pointsWidth ||
		y < 0 || y >= context->pointsHeight)
	{
		return;
	}

	context->removedCount ++;
	points.push_back(
		Vector(
			float(x * context->pointsMult), 
			float(y * context->pointsMult), 
			0.0f));
	
	if (counter && context->removedCount % 50 == 0) 
		counter->setNewPercentage(
			float(context->removedCount)/float(context->pointCount)*100.0f);

	context->wavePoints[x + y * context->pointsWidth] = false;
	if (context->wavePoints[(x+1) + y * context->pointsWidth]) 
		findPath(context, points, x+1, y, counter);
	else if (context->wavePoints[(x-1) + y * context->pointsWidth]) 
		findPath(context, points, x-1, y, counter);
	else if (context->wavePoints[x + (y-1) * context->pointsWidth]) 
		findPath(context, points, x, y-1, counter);
	else if (context->wavePoints[x + (y+1) * context->pointsWidth]) 
		findPath(context, points, x, y+1, counter);
	else if (context->wavePoints[(x-1) + (y-1) * context->pointsWidth]) 
		findPath(context, points, x-1, y-1, counter);
	else if (context->wavePoints[(x-1) + (y+1) * context->pointsWidth]) 
		findPath(context, points, x-1, y+1, counter);
	else if (context->wavePoints[(x+1) + (y+1) * context->pointsWidth]) 
		findPath(context, points, x+1, y+1, counter);
	else if (context->wavePoints[(x+1) + (y-1) * context->pointsWidth]) 
		findPath(context, points, x+1, y-1, counter);
}

void WaterWaves::constructLines(WaterWaveContext *context,
	float waterHeight, std::vector<Vector> &points)
{
	Vector ptA;
	Vector ptB;
	Vector point = points.front();
	int dist = int(RAND * 10.0f + 1.0f);
	for (int i=0; i<(int)points.size(); i++)
	{
		Vector &current = points[i];
		int diffX = int(current[0]) - int(point[0]);
		int diffY = int(current[1]) - int(point[1]);
		int actualdist = diffX * diffX + diffY * diffY;
		if (actualdist  >= dist)
		{
			WaterWaveEntry entry;
			Vector grad = point-current;
			Vector perp = grad.get2DPerp().Normalize2D();

			int newx = int(current[0] + perp[0] * 3.0f);
			int newy = int(current[1] + perp[1] * 3.0f);
			if (newx <= 0 || newy <= 0 ||
				newx >= context->mapWidth || newy >= context->mapHeight)
			{
			}
			else
			{
				if (ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeight(
					newx, newy).asFloat() > waterHeight)
				{
					perp =- perp;
					entry.ptA = current - perp / 3.0f;
					entry.ptB = point - perp / 3.0f;
					entry.perp = perp;
					entry.ptC = point + perp * 6.0f;
					entry.ptD = current + perp * 6.0f;
				}
				else
				{
					entry.ptA = point - perp / 3.0f;
					entry.ptB = current - perp / 3.0f;
					entry.perp = perp;
					entry.ptC = current + perp * 6.0f;
					entry.ptD = point + perp * 6.0f;
				}

				if (RAND > 0.5f) paths1_.push_back(entry);
				else paths2_.push_back(entry);
			}

			if (i > 1)
			{
				i-= 1;
				point = points[i];
			}
			else point = current;
			
			dist = int(RAND * 10.0f + 1.0f);
		}
	}
}

void WaterWaves::simulate(float frameTime)
{
	totalTime_ += frameTime / 2.0f;
	if (totalTime_ > 6.0f) totalTime_ = 0.0f;
}

void WaterWaves::draw(Water2Patches &currentPatch)
{
	//if (OptionsDisplay::instance()->getNoWaves()) return;

	Vector windDir = 
		ScorchedClient::instance()->getSimulator().getWind().getWindStartingDirection().asVector();
	Vector windDirPerp = windDir.Normalize();

	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	wavesTexture1_.draw(true);
	drawBoxes(currentPatch, totalTime_ + 0.0f, windDirPerp, paths1_);
	drawBoxes(currentPatch, totalTime_ + 2.0f, windDirPerp, paths1_);
	drawBoxes(currentPatch, totalTime_ + 4.0f, windDirPerp, paths1_);

	wavesTexture2_.draw(true);
	drawBoxes(currentPatch, totalTime_ + 1.0f, windDirPerp, paths2_);
	drawBoxes(currentPatch, totalTime_ + 3.0f, windDirPerp, paths2_);
	drawBoxes(currentPatch, totalTime_ + 5.0f, windDirPerp, paths2_);

	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void WaterWaves::drawBoxes(Water2Patches &currentPatch,
						   float totalTime, Vector &windDirPerp, 
						   std::vector<WaterWaveEntry> &paths)
{
	float newTime = totalTime;
	if (newTime > 6.0f) newTime -= 6.0f;

	// First set of boxes
	// Some set of magic to try to get it look kind of ok!
	float alpha = 1.0f;
	float frontlen = newTime + 0.2f;
	float endlen = newTime / 2.0f;
	if (newTime < 1.0f)
	{
		alpha = newTime;
	}
	if (newTime > 4.0f)
	{
		frontlen = 4.2f - (newTime - 4.0f) / 3.0f;
		endlen = 2.0f - (newTime - 4.0f) / 3.0f;

		alpha = (2.0f - (newTime - 4.0f)) / 2.0f;
	}
	frontlen *= 2.0f;
	endlen *= 2.0f;

	// Draw the actual texture boxes
	glColor4f(wavesColor_[0], wavesColor_[1], wavesColor_[2], alpha * 0.3f);
	glBegin(GL_QUADS);
		Vector ptA, ptB, ptC, ptD;
		std::vector<WaterWaveEntry>::iterator itor = paths.begin();
		std::vector<WaterWaveEntry>::iterator enditor = paths.end();
		for (; itor != enditor; ++itor)
		{
			WaterWaveEntry &p = *itor;
				
			if (((p.perp[0] * windDirPerp[0]) + (p.perp[1] * windDirPerp[1])) > 0.0f) 
				continue;

			ptA = p.ptD - p.perp * frontlen;
			ptA[2] = currentPatch.getPoint(int(ptA[0]/2), int(ptA[1]/2))->z + 0.05f;
			ptB = p.ptC - p.perp * frontlen;
			ptB[2] = currentPatch.getPoint(int(ptB[0]/2), int(ptB[1]/2))->z + 0.05f;

			ptC = p.ptC - p.perp * endlen;
			ptC[2] = currentPatch.getPoint(int(ptC[0]/2), int(ptC[1]/2))->z + 0.05f;
			ptD = p.ptD - p.perp * endlen;
			ptD[2] = currentPatch.getPoint(int(ptD[0]/2), int(ptD[1]/2))->z + 0.05f;
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(ptA);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(ptB);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(ptC);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(ptD);
		}
	glEnd();
	GLInfo::addNoTriangles((unsigned int) paths.size() - 2);
}
