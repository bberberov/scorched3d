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

#include <placement/PlacementType.hpp>
#include <placement/PlacementObject.hpp>
#include <placement/PlacementTypeMask.hpp>
#include <placement/PlacementTypeDirect.hpp>
#include <placement/PlacementTypeTree.hpp>
#include <placement/PlacementTypeCount.hpp>
#include <placement/PlacementTypeBounds.hpp>
#include <placement/PlacementTypeTankStart.hpp>
#include <landscapemap/LandscapeMaps.hpp>
#include <target/Target.hpp>
#include <target/TargetContainer.hpp>
#include <target/TargetLife.hpp>
#include <engine/ScorchedContext.hpp>
#include <common/DefinesString.hpp>
#include <XML/XMLParser.hpp>

PlacementType *PlacementType::create(const char *type)
{
	if (0 == strcmp(type, "trees")) return new PlacementTypeTree;
	if (0 == strcmp(type, "mask")) return new PlacementTypeMask;
	if (0 == strcmp(type, "direct")) return new PlacementTypeDirect;
	if (0 == strcmp(type, "count")) return new PlacementTypeCount;
	if (0 == strcmp(type, "bounds")) return new PlacementTypeBounds;
	if (0 == strcmp(type, "tankstart")) return new PlacementTypeTankStart;
	S3D::dialogMessage("PlacementType", S3D::formatStringBuffer("Unknown placement type %s", type));
	return nullptr;
}

PlacementType::PlacementType() : placementobject( nullptr ) {}

PlacementType::~PlacementType()
{
	delete placementobject;
	placementobject = nullptr;
}

bool PlacementType::readXML(XMLNode *node)
{
	std::string objecttype;
	XMLNode *objectNode;
	if (!node->getNamedChild("object", objectNode)) return false;
	if (!objectNode->getNamedParameter("type", objecttype)) return false;
	if (!(placementobject = PlacementObject::create(objecttype.c_str()))) return false;
	if (!placementobject->readXML(objectNode)) return false;
	return node->failChildren();
}

void PlacementType::createObjects(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	ProgressCounter *counter)
{
	std::list<Position> returnPositions;
	getPositions(context, generator, returnPositions, counter);

	std::list<Position>::iterator itor;
	for (itor = returnPositions.begin();
		itor != returnPositions.end();
		++itor)
	{
		Position &position = *itor;
		placementobject->createObject(
			context, generator, playerId, position);
	}
}

bool PlacementType::checkCloseness(FixedVector &position,
	ScorchedContext &context,
	std::list<Position> &returnPositions,
	fixed mincloseness)
{
	// Check for border closeness for targets
	std::map<unsigned int, Target *> &targets = 
		context.getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator targetsitor;
	for (targetsitor = targets.begin();
		targetsitor != targets.end();
		++targetsitor)
	{
		Target *target = (*targetsitor).second;
		if (target->getType() == Target::TypeTank) continue;

		fixed distx = target->getLife().getTargetPosition()[0] - position[0];
		fixed disty = target->getLife().getTargetPosition()[1] - position[1];
		fixed closeness = mincloseness + target->getBorder();

		if (closeness > 0)
		{
			if (distx.abs() < closeness &&
				disty.abs() < closeness)
			{
				return false;
			}
		}
	}

	// Check for mincloseness closeness for objects in the current group
	if (mincloseness > 0)
	{
		std::list<Position>::iterator currentItor;
		for (currentItor = returnPositions.begin();
			currentItor != returnPositions.begin();
			++currentItor)
		{
			Position *object = &(*currentItor);

			fixed distx = object->position[0] - position[0];
			fixed disty = object->position[1] - position[1];

			if (distx.abs() < mincloseness &&
				disty.abs() < mincloseness)
			{
				return false;
			}	
		}
	}

	return true;
}
