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

#include <tanket/TanketTypes.h>
#include <common/Defines.h>
#include <XML/XMLFile.h>

TanketTypes::TanketTypes() : defaultType_(0)
{
}

TanketTypes::~TanketTypes()
{
	clear();
}

bool TanketTypes::loadTanketTypes(ScorchedContext &context)
{
	clear();

	XMLFile file;
	if (!file.readFile(S3D::getModFile("data/tanktypes.xml")))
	{
		S3D::dialogMessage("Scorched3D", 
			S3D::formatStringBuffer("Failed to parse data/TanketTypes.xml\n%s", 
			file.getParserError()));
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		S3D::dialogMessage("Scorched3D",
			"Failed to find tank definition file \"data/tanktypes.xml\"");
		return false;		
	}

	// Itterate all of the TanketType in the file
	XMLNode *currentNode = 0;
	while (file.getRootNode()->getNamedChild("tanktype", currentNode, false))
    {
		// Create the TanketType
		TanketType *type = new TanketType();
		if (!type->initFromXML(context, currentNode)) return false;
		types_.push_back(type);
		if (type->getUseAsDefault()) defaultType_ = type;
	}

	if (types_.empty())
	{
		return file.getRootNode()->returnError(
			"tank types file must define at least one type");
	}
	if (!defaultType_)
	{
		defaultType_ = types_.front();
	}

	return file.getRootNode()->failChildren();
}

TanketType *TanketTypes::getType(const char *name)
{
	std::vector<TanketType *>::iterator itor;
	for (itor = types_.begin();
		itor != types_.end();
		++itor)
	{
		TanketType *type = (*itor);
		if (0 == strcmp(name, type->getName())) return type;
	}
	return 0;
}

void TanketTypes::clear()
{
	defaultType_ = 0;
	while (!types_.empty())
	{
		delete types_.back();
		types_.pop_back();
	}

	types_.clear();
}
