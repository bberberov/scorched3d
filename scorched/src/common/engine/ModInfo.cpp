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

#include <engine/ModInfo.h>
#include <common/Defines.h>
#include <XML/XMLFile.h>

ModInfo::ModInfo(const std::string &name) :
	name_(name)
{

}

ModInfo::~ModInfo()
{
}

bool ModInfo::parse(const std::string &fileName)
{
	entries_.clear();
	XMLFile file;
	if (!file.readFile(fileName))
	{
		S3D::dialogMessage("ModInfo", S3D::formatStringBuffer(
			"Failed to parse \"%s\":%s\n", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		S3D::dialogMessage("ModInfo", S3D::formatStringBuffer(
					  "Failed to find mod info definition file \"%s\"",
					  fileName.c_str()));
		return false;		
	}

	// Parse the main mod info
	std::string tmpicon, tmpgamefile;
	XMLNode *mainNode = 0;
	if (!file.getRootNode()->getNamedChild("main", mainNode)) return false;
	if (!mainNode->getNamedChild("description", description_)) return false;
	if (!mainNode->getNamedChild("icon", tmpicon)) return false;
	if (!mainNode->getNamedChild("url", url_)) return false;
	if (!mainNode->getNamedChild("protocolversion", protocolversion_)) return false;
	if (!mainNode->getNamedChild("shortdescription", shortDescription_, false))
	{
		shortDescription_ = description_;
	}
	if (!mainNode->failChildren()) return false;

	if (S3D::fileExists(S3D::getModFile(tmpicon)))
	{
		icon_ = S3D::getModFile(tmpicon);
	}
	else
	{
		icon_ = S3D::getDataFile("data/images/tank2.bmp");
	}

	// Parse the mod game info
	XMLNode *gameNode = 0;
	while (file.getRootNode()->getNamedChild("game", gameNode, false))
	{
		MenuEntry entry;
		if (!gameNode->getNamedChild("description", entry.description)) return false;
		if (!gameNode->getNamedChild("icon", tmpicon)) return false;
		if (!gameNode->getNamedChild("gamefile", tmpgamefile)) return false;
		if (!gameNode->getNamedChild("shortdescription", entry.shortdescription, false))
		{
			entry.shortdescription = entry.description;
		}

		if (S3D::fileExists(S3D::getModFile(tmpicon))) 
		{
			entry.icon = S3D::getModFile(tmpicon);
		}
		else
		{
			entry.icon = S3D::getDataFile("data/images/tank2.bmp");
		}
	
		entry.gamefile = S3D::getModFile(tmpgamefile);
		if (!S3D::checkDataFile(tmpgamefile)) return false;

		if (!gameNode->failChildren()) return false;
		entries_.push_back(entry);
	}

	return file.getRootNode()->failChildren();
}
