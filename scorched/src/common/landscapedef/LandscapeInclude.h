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

#if !defined(__INCLUDE_LandscapeIncludeh_INCLUDE__)
#define __INCLUDE_LandscapeIncludeh_INCLUDE__

#include <placement/PlacementType.h>
#include <vector>

class LandscapeEvent;
class LandscapeMovementType;
class PlacementType;
class LandscapeSoundType;
class LandscapeMusicType;
class LandscapeOptionsType;
class LandscapeDefinitions;
class LandscapeInclude
{
public:
	LandscapeInclude();
	virtual ~LandscapeInclude();

	std::vector<LandscapeEvent *> events;
	std::vector<LandscapeMovementType *> movements;
	std::vector<PlacementType *> placements;
	std::vector<LandscapeSoundType *> sounds;
	std::vector<LandscapeMusicType *> musics;
	std::vector<LandscapeOptionsType *> options;

	bool readXML(LandscapeDefinitions *definitions, XMLNode *node);

private:
	LandscapeInclude(const LandscapeInclude &other);
	LandscapeInclude &operator=(LandscapeInclude &other);
};

#endif // __INCLUDE_LandscapeIncludeh_INCLUDE__
