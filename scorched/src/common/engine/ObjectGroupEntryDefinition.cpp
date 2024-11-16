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

#include <engine/ObjectGroupEntryDefinition.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroups.h>

ObjectGroupEntryDefinition::ObjectGroupEntryDefinition()
{
}

ObjectGroupEntryDefinition::~ObjectGroupEntryDefinition()
{
}

bool ObjectGroupEntryDefinition::readXML(XMLNode *node, const std::string &nodeName)
{
	std::string groupname;
	while (node->getNamedChild(nodeName.c_str(), groupname, false))
	{
		groupnames_.push_back(groupname);
	}
	return true;
}

void ObjectGroupEntryDefinition::addToGroups(
	ObjectGroups &objectGroups,
	ObjectGroupEntry *objectGroupEntry)
{
	for (unsigned int i=0; i<groupnames_.size(); i++)
	{
		std::string groupname = groupnames_[i];
		addToGroup(groupname.c_str(), objectGroups, objectGroupEntry);
	}
}

void ObjectGroupEntryDefinition::addToGroup(
	const char *groupName,
	ObjectGroups &objectGroups,
	ObjectGroupEntry *objectGroupEntry)
{
	ObjectGroup *group = objectGroups.getGroup(groupName, true);
	if (group)
	{
		group->addObject(objectGroupEntry);
	}
}
