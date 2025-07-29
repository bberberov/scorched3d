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

#if !defined(__INCLUDE_LanscapeMapsh_INCLUDE__)
#define __INCLUDE_LanscapeMapsh_INCLUDE__

#include <landscapemap/RoofMaps.h>
#include <landscapemap/GroundMaps.h>
#include <landscapedef/LandscapeDefinitionCache.h>

class ScorchedClient;
class LandscapeMaps
{
public:
	LandscapeMaps();
	virtual ~LandscapeMaps();

	void generateMaps(
		ScorchedContext &context,
		LandscapeDefinition &defn,
		ProgressCounter *counter = 0);

	GroundMaps &getGroundMaps() { return gMaps_; }
	RoofMaps &getRoofMaps() { return rMaps_; }
	LandscapeDefinitionCache &getDefinitions() { return dCache_; }

protected:
	LandscapeDefinitionCache dCache_;
	GroundMaps gMaps_;
	RoofMaps rMaps_;
};

#endif
