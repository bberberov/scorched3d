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

#include <engine/ObjectGroupEntry.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <net/NetBuffer.h>
#include <set>

ObjectGroupEntry::ObjectGroupEntry(ObjectGroups &groups) :
	groups_(groups)
{
}

ObjectGroupEntry::~ObjectGroupEntry()
{
	removeFromAllGroups();
}

bool ObjectGroupEntry::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "ObjectGroupEntry");
	buffer.addToBufferNamed("size", (int) containment_.size());

	std::set<std::string> groupNames;
	{
		std::set<ObjectGroup *>::iterator itor;
		for (itor = containment_.begin();
			itor != containment_.end();
			++itor)
		{
			ObjectGroup *group = *itor;
			groupNames.insert(group->getName());
		}	
	}
	{
		std::set<std::string>::iterator itor;
		for (itor = groupNames.begin();
			itor != groupNames.end();
			++itor)
		{			
			buffer.addToBufferNamed("name", *itor);
		}
	}

	return true;
}

bool ObjectGroupEntry::readMessage(NetBufferReader &reader)
{
	std::set<std::string> goodGroups;

	int groupNo = 0;
	if (!reader.getFromBuffer(groupNo)) return false;
	for (int g=0; g<groupNo; g++)
	{
		std::string groupName;
		if (!reader.getFromBuffer(groupName)) return false;

		ObjectGroup *group = groups_.getGroup(groupName.c_str());
		if (group && !group->hasObject(this))
		{
			group->addObject(this);
		}
		goodGroups.insert(groupName);
	}

	std::set<ObjectGroup *> groupsCopy = containment_;
	std::set<ObjectGroup *>::iterator itor;
	for (itor = groupsCopy.begin();
		itor != groupsCopy.end();
		++itor)
	{
		ObjectGroup *group = *itor;
		if (goodGroups.find(group->getName()) == goodGroups.end())
		{
			if (group->hasObject(this))
			{
				group->removeObject(this);
			}
		}
	}

	return true;
}

void ObjectGroupEntry::addToGroup(ObjectGroup *group)
{
	containment_.insert(group);
}

void ObjectGroupEntry::removeFromGroup(ObjectGroup *group)
{
	containment_.erase(group);
}

void ObjectGroupEntry::removeFromAllGroups()
{
	if (!containment_.empty())
	{
		std::set<ObjectGroup *> groupsCopy = containment_;
		std::set<ObjectGroup *>::iterator itor;
		for (itor = groupsCopy.begin();
			itor != groupsCopy.end();
			++itor)
		{
			ObjectGroup *group = *itor;
			group->removeObject(this);
		}
		containment_.clear();
	}
	if (!references_.empty())
	{
		std::set<ObjectGroupEntryReference *>::iterator itor;
		for (itor = references_.begin();
			itor != references_.end();
			++itor)
		{
			(*itor)->clearEntry();
		}
		references_.clear();
	}
}
