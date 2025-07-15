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

#include <XML/XMLFile.h>
#include <common/Defines.h>
#include <tankai/TankAINames.h>
#include <stdlib.h>

TankAINames::TankAINames()
{

}

TankAINames::~TankAINames()
{

}

bool TankAINames::loadAIs(std::list<std::string> &ais)
{
	// Load key definition file
	XMLFile file;
	if (!file.readFile(S3D::getModFile("data/tankais.xml")))
	{
		S3D::dialogMessage("TankAIStore", 
					  S3D::formatStringBuffer("Failed to parse \"%s\"\n%s", 
					  "data/tankais.xml",
					  file.getParserError()));
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		S3D::dialogMessage("TankAIStore",
					  S3D::formatStringBuffer("Failed to find tank ai definition file \"%s\"",
					  "data/tankais.xml"));
		return false;		
	}

	// Itterate all of the keys in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 ++childrenItor)
    {
		// Parse the ai entry
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "ai"))
		{
			S3D::dialogMessage("TankAIStore",
						  "Failed to find ai node");
			return false;
		}

		std::string name;
		if (!currentNode->getNamedChild("name", name)) return false;

		bool availableForPlayers = true;
		currentNode->getNamedChild("availableforplayers", availableForPlayers, false);

		if (availableForPlayers)
		{
			ais.push_back(name);
		}
	}

	ais.push_back("Random");
	ais.push_back("Human");

	return true;
}
