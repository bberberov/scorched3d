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

#include <placement/PlacementObjectRandom.h>
#include <common/RandomGenerator.h>
#include <XML/XMLParser.h>

PlacementObjectRandom::PlacementObjectRandom() : totalWeight_(0)
{
}

PlacementObjectRandom::~PlacementObjectRandom()
{
}

bool PlacementObjectRandom::readXML(XMLNode *initialNode)
{
	XMLNode *node;
	while (initialNode->getNamedChild("randomobject", node, false))
	{
		RandomObject randomObject;

		// Get the weight
		randomObject.weight = 1;
		node->getNamedChild("weight", randomObject.weight, false);
		totalWeight_ += randomObject.weight;

		// Get the object
		std::string objecttype;
		XMLNode *objectNode;
		if (!node->getNamedChild("object", objectNode)) return false;
		if (!objectNode->getNamedParameter("type", objecttype)) return false;
		if (!(randomObject.object = PlacementObject::create(objecttype.c_str()))) return false;
		if (!randomObject.object->readXML(objectNode)) return false;

		objects_.push_back(randomObject);
	}
	if (!node->failChildren()) return false;

	return PlacementObject::readXML(node);
}

void PlacementObjectRandom::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	fixed totalWeight = generator.getRandFixed("PlacementObjectRandom") * totalWeight_;
	fixed currentWeight = 0;

	std::vector<RandomObject>::iterator itor;
	for (itor = objects_.begin();
		itor != objects_.end();
		++itor)
	{
		RandomObject &object = (*itor);
		currentWeight += object.weight;

		if (currentWeight > totalWeight)
		{
			PlacementObject *entry = object.object;
			entry->createObject(context, generator, playerId, position);
			break;
		}
	}
}
