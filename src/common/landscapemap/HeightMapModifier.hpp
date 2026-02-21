////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_HeightMapModifier_hpp_INCLUDE__
#define __INCLUDE_HeightMapModifier_hpp_INCLUDE__

#include <common/RandomGenerator.hpp>
#include <common/FixedVector.hpp>
#include <landscapemap/HeightMap.hpp>

class LandscapeDefnHeightMapGenerate;

namespace HeightMapModifier  
{
	void levelSurround(HeightMap &hmap);
	void noise(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		RandomGenerator &generator,
		ProgressCounter *counter = 0);
	void edgeEnhance(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		RandomGenerator &generator,
		ProgressCounter *counter = 0);
	void waterErrosion(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		RandomGenerator &generator,
		ProgressCounter *counter = 0);
	void smooth(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		ProgressCounter *counter = 0);
	void scale(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		RandomGenerator &generator,
		ProgressCounter *counter = 0);

	void generateTerrain(HeightMap &hmap, 
		LandscapeDefnHeightMapGenerate &defn,
		RandomGenerator &generator, 
		RandomGenerator &offsetGenerator,
		ProgressCounter *counter = 0);
	void addCirclePeak(HeightMap &hmap, FixedVector &start, 
		fixed sizew, fixed sizew2, fixed sizeh,
		RandomGenerator &generator);
};

#endif // __INCLUDE_HeightMapModifier_hpp_INCLUDE__
