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

#include <placement/PlacementTypeTankStart.h>
#include <placement/PlacementTankPosition.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>

PlacementTypeTankStart::PlacementTypeTankStart() : mincloseness(0)
{
}

PlacementTypeTankStart::~PlacementTypeTankStart()
{
}

bool PlacementTypeTankStart::readXML(XMLNode *node)
{
	if (!node->getNamedChild("numobjects", numobjects)) return false;
	if (!node->getNamedChild("team", team)) return false;
	node->getNamedChild("mincloseness", mincloseness, false);
	return PlacementType::readXML(node);
}

void PlacementTypeTankStart::getPositions(ScorchedContext &context,
	RandomGenerator &generator,
	std::list<Position> &returnPositions,
	ProgressCounter *counter)
{
	for (int i=0; i<numobjects;)
	{
		Position position;
		position.position = 
			PlacementTankPosition::placeTank(0, team, context, generator);

		if (checkCloseness(position.position, context, 
			returnPositions, mincloseness))
		{
			returnPositions.push_back(position);
			i++;
		}
	}
}
