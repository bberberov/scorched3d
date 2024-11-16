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

#include <placement/PlacementTypeBounds.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

PlacementTypeBounds::PlacementTypeBounds() : onground(false)
{
}

PlacementTypeBounds::~PlacementTypeBounds()
{
}

bool PlacementTypeBounds::readXML(XMLNode *node)
{
	if (!node->getNamedChild("count", count)) return false;
	if (!node->getNamedChild("minbounds", minbounds)) return false;
	if (!node->getNamedChild("maxbounds", maxbounds)) return false;

	node->getNamedChild("onground", onground, false);

	if (maxbounds[0] - minbounds[0] < 25 ||
		maxbounds[1] - minbounds[1] < 25 ||
		maxbounds[2] - minbounds[2] < 10)
	{
		return node->returnError(
			"PlacementTypeBounds bounding box is too small, it must be at least 25x25x10 units");
	}

	return PlacementType::readXML(node);
}

void PlacementTypeBounds::getPositions(ScorchedContext &context,
	RandomGenerator &generator,
	std::list<Position> &returnPositions,
	ProgressCounter *counter)
{
	for (int i=0; i<count; i++)
	{
		// Set the position, and make sure it is above ground
		Position position;
		do
		{
			position.position[0] = 
				generator.getRandFixed("PlacementTypeBounds") * (maxbounds[0] - minbounds[0]) + minbounds[0];
			position.position[1] = 
				generator.getRandFixed("PlacementTypeBounds") * (maxbounds[1] - minbounds[1]) + minbounds[1];

			fixed groundHeight = context.getLandscapeMaps().getGroundMaps().
				getInterpHeight(position.position[0], position.position[1]);

			if (onground) 
			{
				position.position[2] = groundHeight;
			} 
			else
			{
				position.position[2] = 
					generator.getRandFixed("PlacementTypeBounds") * (maxbounds[2] - minbounds[2]) + minbounds[2];
			}

			if (position.position[2] >= groundHeight) break;
		} 
		while (true);

		// Set velocity pointing into the middle
		FixedVector middle = (maxbounds + minbounds) / 2;
		position.velocity = middle - position.position;
		position.velocity.StoreNormalize();
		position.velocity[2] = 0;

		returnPositions.push_back(position);
	}
}
