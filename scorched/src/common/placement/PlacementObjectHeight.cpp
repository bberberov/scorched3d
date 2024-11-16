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

#include <placement/PlacementObjectHeight.h>
#include <common/RandomGenerator.h>
#include <XML/XMLParser.h>

PlacementObjectHeight::PlacementObjectHeight()
{
}

PlacementObjectHeight::~PlacementObjectHeight()
{
}

bool PlacementObjectHeight::readXML(XMLNode *initialNode)
{
	XMLNode *node;
	while (initialNode->getNamedChild("heightobject", node, false))
	{
		HeightObject heightObject;

		if (!node->getNamedChild("minheight", heightObject.min)) return false;
		if (!node->getNamedChild("maxheight", heightObject.max)) return false;

		// Get the object
		std::string objecttype;
		XMLNode *objectNode;
		if (!node->getNamedChild("object", objectNode)) return false;
		if (!objectNode->getNamedParameter("type", objecttype)) return false;
		if (!(heightObject.object = PlacementObject::create(objecttype.c_str()))) return false;
		if (!heightObject.object->readXML(objectNode)) return false;

		objects_.push_back(heightObject);
	}
	if (!node->failChildren()) return false;

	return PlacementObject::readXML(node);
}

void PlacementObjectHeight::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	fixed offset = generator.getRandFixed("PlacementObjectHeight") * 10 - 5;
	fixed height = position.position[2] + offset;

	std::vector<HeightObject>::iterator itor;
	for (itor = objects_.begin();
		itor != objects_.end();
		++itor)
	{
		HeightObject &object = (*itor);

		if (height >= object.min &&
			height <= object.max)
		{
			PlacementObject *entry = object.object;
			entry->createObject(context, generator, playerId, position);
			break;
		}
	}
}
