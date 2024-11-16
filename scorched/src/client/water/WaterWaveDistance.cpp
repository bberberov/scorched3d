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

#include <water/WaterWaveDistance.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <lang/LangResource.h>
#include <common/ProgressCounter.h>

WaterWaveDistance::WaterWaveDistance() : waveDistance_(0)
{
}

WaterWaveDistance::~WaterWaveDistance()
{
}

void WaterWaveDistance::generate(
	int mapWidth, int mapHeight, float waterHeight,
	ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("WAVES_DISTANCE", "Waves Distance"));

	// Wave distance
	distanceWidthMult_ = 4;
	distanceHeightMult_ = 4;
	distanceWidth_ = mapWidth / distanceWidthMult_;
	distanceHeight_ = mapHeight / distanceHeightMult_;

	delete [] waveDistance_;
	waveDistance_ = new float[distanceWidth_ * distanceHeight_];
	for (int a=0; a<distanceWidth_; a++)
	{
		for (int b=0; b<distanceHeight_; b++)
		{
			waveDistance_[a + b * distanceWidth_] = 255.0f;
		}
	}

	const int skip = distanceWidthMult_;
	for (int y=0; y<mapHeight; y+=skip)
	{
		if (counter) counter->setNewPercentage(float(y)/
			float(mapHeight)*100.0f);

		for (int x=0; x<mapWidth; x+=skip)
		{
			float height =
				ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y).asFloat();
			if (height > waterHeight - 4.0f && height < waterHeight)
			{
				float height2=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x+skip, y).asFloat();
				float height3=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x-skip, y).asFloat();
				float height4=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y+skip).asFloat();
				float height5=
					ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight(x, y-skip).asFloat();

				if (height2 > waterHeight || height3 > waterHeight ||
					height4 > waterHeight || height5 > waterHeight)
				{
					Vector posA((float) x, (float) y, 0.0f);
					for (int b=0; b<distanceHeight_; b++)
					{
						for (int a=0; a<distanceWidth_; a++)
						{
							Vector posB(
								float(float(a) / float(distanceWidth_) * float(mapWidth)), 
								float(float(b) / float(distanceHeight_) * float(mapHeight)), 0.0f);
							float distance = (posB - posA).Magnitude();
							waveDistance_[a + b * distanceWidth_] = MIN(waveDistance_[a + b * distanceWidth_], distance);
						}
					}
				}

			}
		}
	}
}

float WaterWaveDistance::getWaveDistance(int posx, int posy)
{
	int a = int(posx) / distanceWidthMult_;
	int b = int(posy) / distanceHeightMult_;
	int x = MAX(0, MIN(a, (distanceWidth_ - 1)));
	int y = MAX(0, MIN(b, (distanceHeight_ - 1)));

	float distance = waveDistance_[x + y * distanceWidth_];
	distance += fabsf(float(a - x)) + fabsf(float(b - y));
	return distance;
}
