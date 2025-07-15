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

#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/DeformLandscape.h>
#include <engine/ScorchedContext.h>
#include <engine/ScorchedContext.h>
#include <movement/TargetMovement.h>
#ifndef S3D_SERVER
	#include <land/VisibilityPatchGrid.h>
#endif

LandscapeMaps::LandscapeMaps() :
	gMaps_(dCache_), rMaps_(dCache_)
{
}

LandscapeMaps::~LandscapeMaps()
{
}

void LandscapeMaps::generateMaps(
	ScorchedContext &context,
	LandscapeDefinition &defn,
	ProgressCounter *counter)
{
	dCache_.setDefinition(context, defn);
	gMaps_.generateMaps(context, counter);
	rMaps_.generateMaps(context, counter);
#ifndef S3D_SERVER
	if (!context.getServerMode())
	{
		VisibilityPatchGrid::instance()->generate();
	}
#endif
	gMaps_.generateObjects(context, counter);

	// Create movement after targets, so we can mark 
	// those targets that are in movement groups
	context.getTargetMovement().generate(context); 
}

