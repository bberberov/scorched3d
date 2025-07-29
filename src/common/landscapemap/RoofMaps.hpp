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

#if !defined(__INCLUDE_RoofMapsh_INCLUDE__)
#define __INCLUDE_RoofMapsh_INCLUDE__

#include <landscapemap/HeightMap.h>

class ScorchedContext;
class LandscapeDefinitionCache;
class RoofMaps
{
public:
	RoofMaps(LandscapeDefinitionCache &defnCache);
	virtual ~RoofMaps();

	void generateMaps(
		ScorchedContext &context,
		ProgressCounter *counter = 0);

	fixed getRoofHeight(int x, int y);
	fixed getInterpRoofHeight(fixed x, fixed y);

	bool getRoofOn();
	fixed getRoofBaseHeight() { return roofBaseHeight_; }

	HeightMap &getRoofMap() { return rmap_; }
	HeightMap &getDeformRoofMap() { return deformRMap_; }

protected:
	LandscapeDefinitionCache &defnCache_;

	// The roof of the landscape
	HeightMap rmap_;
	HeightMap deformRMap_;
	fixed roofBaseHeight_;

	void generateRMap(
		ScorchedContext &context,
		ProgressCounter *counter = 0);
};

#endif // __INCLUDE_RoofMapsh_INCLUDE__
