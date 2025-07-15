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

#include <landscapedef/LandscapeDefinitionsBase.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool LandscapeDefinitionsEntry::readXML(XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	if (!node->getNamedChild("weight", weight)) return false;
	if (!node->getNamedChild("description", description)) return false;
	if (!node->getNamedChild("picture", picture)) return false;

	XMLNode *tex, *defn, *tmp;
	if (!node->getNamedChild("defn", defn)) return false;
	while (defn->getNamedChild("item", tmp, false, true))
	{
		const char *landscapeDefnFile = tmp->getContent();
		defns.push_back(landscapeDefnFile);
	}
	if (!node->getNamedChild("tex", tex)) return false;
	while (tex->getNamedChild("item", tmp, false, true))
	{
		const char *landscapeTexFile = tmp->getContent();
		texs.push_back(landscapeTexFile);
	}

	DIALOG_ASSERT(!texs.empty() && !defns.empty());
	return node->failChildren();
}

LandscapeDefinitionsBase::LandscapeDefinitionsBase()
{
}

LandscapeDefinitionsBase::~LandscapeDefinitionsBase()
{
}

void LandscapeDefinitionsBase::clearLandscapeDefinitions()
{
	entries_.clear();
}

bool LandscapeDefinitionsBase::readLandscapeDefinitions()
{
	// Load landscape definition file
	XMLFile file;
	if (!file.readFile(S3D::getModFile("data/landscapes.xml")) ||
		!file.getRootNode())
	{
		S3D::dialogMessage("Scorched Landscape", S3D::formatStringBuffer(
					  "Failed to parse \"data/landscapes.xml\"\n%s", 
					  file.getParserError()));
		return false;
	}

	// Itterate all of the landscapes in the file
	std::list<XMLNode *>::iterator childrenItor;
		std::list<XMLNode *> &children = file.getRootNode()->getChildren();
	for (childrenItor = children.begin();
		childrenItor != children.end();
		++childrenItor)
	{
		LandscapeDefinitionsEntry newDefn;
		if (!newDefn.readXML(*childrenItor)) return false;
		entries_.push_back(newDefn);
	}
	return true;
}

bool LandscapeDefinitionsBase::landscapeEnabled(OptionsGame &context, 
											const char *name)
{
	std::string landscapes = context.getLandscapes();
	if (landscapes.empty()) return true; // Default un-initialized state

	char *token = strtok((char *) landscapes.c_str(), ":");
	while(token != 0)
	{
		if (0 == strcmp(token, name)) return true;
		token = strtok(0, ":");
	}
	return false;
}

LandscapeDefinitionsEntry *LandscapeDefinitionsBase::getLandscapeByName(
	const char *name)
{
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		LandscapeDefinitionsEntry &result = *itor;
		if (0 == strcmp(name, result.name.c_str()))
		{
			return &result;
		}
	}
	return 0;
}
