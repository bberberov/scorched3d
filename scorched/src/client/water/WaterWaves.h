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

#if !defined(__INCLUDE_WaterWavesh_INCLUDE__)
#define __INCLUDE_WaterWavesh_INCLUDE__

#include <common/Vector.h>
#include <common/ProgressCounter.h>
#include <GLEXT/GLTexture.h>
#include <vector>

class Water2Patches;
class WaterWaves
{
public:
	WaterWaves();
	virtual ~WaterWaves();

	void generateWaves(float waterHeight, ProgressCounter *counter = 0);
	void draw(Water2Patches &currentPatch);
	void simulate(float frameTime);

protected:
	struct WaterWaveEntry 
	{
		Vector perp;
		Vector ptA;
		Vector ptB;
		Vector ptC;
		Vector ptD;
	};
	struct WaterWaveContext
	{
		int mapWidth;
		int mapHeight;
		int pointsWidth;
		int pointsHeight;
		int pointsMult;
		bool *wavePoints;
		unsigned int pointCount;
		unsigned int removedCount;
	};

	GLTexture wavesTexture1_;
	GLTexture wavesTexture2_;
	Vector wavesColor_;

	std::vector<WaterWaveEntry> paths1_;
	std::vector<WaterWaveEntry> paths2_;
	float totalTime_;

	void findPoints(WaterWaveContext *context,
		float waterHeight, ProgressCounter *counter);
	bool findNextPath(WaterWaveContext *context,
		float waterHeight, ProgressCounter *counter);
	void findPath(WaterWaveContext *context,
		std::vector<Vector> &points, int x, int y, ProgressCounter *counter);
	void constructLines(WaterWaveContext *context,
		float waterHeight, std::vector<Vector> &points);
	void drawBoxes(Water2Patches &currentPatch,
		float totalTime, Vector &windDir, 
		std::vector<WaterWaveEntry> &paths);

};

#endif
