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

#include <placement/PlacementObjectGroup.h>
#include <XML/XMLParser.h>

PlacementObjectGroup::PlacementObjectGroup()
{
}

PlacementObjectGroup::~PlacementObjectGroup()
{
}

bool PlacementObjectGroup::readXML(XMLNode *initialNode)
{
	XMLNode *node;
	while (initialNode->getNamedChild("groupobject", node, false))
	{
		GroupObject groupObject;

		if (!node->getNamedChild("offset", groupObject.offset)) return false;

		std::string objecttype;
		XMLNode *objectNode;
		if (!node->getNamedChild("object", objectNode)) return false;
		if (!objectNode->getNamedParameter("type", objecttype)) return false;
		if (!(groupObject.object = PlacementObject::create(objecttype.c_str()))) return false;
		if (!groupObject.object->readXML(objectNode)) return false;

		groups_.push_back(groupObject);
	}
	if (!node->failChildren()) return false;

	return PlacementObject::readXML(node);
}

void PlacementObjectGroup::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	std::list<GroupObject>::iterator itor;
	for (itor = groups_.begin();
		itor != groups_.end();
		++itor)
	{
		GroupObject &groupObject = (*itor);

		PlacementType::Position newPosition = position;
		newPosition.position += groupObject.offset;
		groupObject.object->createObject(context, 
			generator, 
			playerId, 
			newPosition);
	}
}
