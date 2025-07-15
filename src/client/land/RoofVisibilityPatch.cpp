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

#include <land/RoofVisibilityPatch.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>

RoofVisibilityPatch::RoofVisibilityPatch() :
	HeightMapVisibilityPatch(&ScorchedClient::instance()->getLandscapeMaps().
		getRoofMaps().getRoofMap())
{
}

RoofVisibilityPatch::~RoofVisibilityPatch()
{
}

void RoofVisibilityPatch::calculateErrors()
{
	HeightMapVisibilityPatch::calculateErrors();

	// Make sure edge patches are at their highest settings
	// so they blend in with the cavern border
	if (!leftPatch_ || !rightPatch_ || !topPatch_ || !bottomPatch_)
	{
		for (int i=0;i<5; i++) indexErrors_[i] = 10000.0f;
	}
}
