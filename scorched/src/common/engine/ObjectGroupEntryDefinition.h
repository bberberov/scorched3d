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

#if !defined(__INCLUDE_ObjectGroupEntryDefinitionh_INCLUDE__)
#define __INCLUDE_ObjectGroupEntryDefinitionh_INCLUDE__

#include <XML/XMLNode.h>
#include <vector>
#include <string>

class ObjectGroupEntry;
class ObjectGroups;
class ObjectGroupEntryDefinition
{
public:
	ObjectGroupEntryDefinition();
	virtual ~ObjectGroupEntryDefinition();

	bool readXML(XMLNode *node, const std::string &nodeName);

	bool hasGroups() { return !groupnames_.empty(); }
	void addToGroups(ObjectGroups &objectGroups,
		ObjectGroupEntry *objectGroupEntry);

protected:
	std::vector<std::string> groupnames_;

	void addToGroup(const char *groupName,
		ObjectGroups &objectGroups,
		ObjectGroupEntry *objectGroupEntry);
};

#endif // __INCLUDE_ObjectGroupEntryDefinitionh_INCLUDE__
