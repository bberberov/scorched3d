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

#include <client/UniqueIdStore.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <graph/OptionsDisplay.h>
#include <net/NetInterface.h>
#include <XML/XMLFile.h>
#include <stdlib.h>

UniqueIdStore::UniqueIdStore()
{
	DIALOG_ASSERT(SDLNet_Init()!=-1);
}

UniqueIdStore::~UniqueIdStore()
{
}

bool UniqueIdStore::loadStore()
{
	ids_.clear();

	// Parse the XML file
	std::string idsPath = S3D::getSettingsFile("ids.xml");
	XMLFile file;
	if (!file.readFile(idsPath))
	{
		S3D::dialogMessage("Scorched3D Ids", 
			S3D::formatStringBuffer("ERROR: Failed to parse file \"%s\"\n"
			"%s",
			idsPath.c_str(),
			file.getParserError()));		
		return false;
	}

	// return true for an empty file
	if (!file.getRootNode()) return true;

	XMLNode *node;
	while (file.getRootNode()->getNamedChild("id", node, false))
	{
		Entry entry;
		if (!node->getNamedChild("id", entry.id)) return false;
		if (!node->getNamedChild("published", entry.published)) return false;

		if (0 == strcmp(entry.published.c_str(), "scorched3d.game-host.org"))
		{
			entry.published = "game-host.scorched3d.co.uk";
		}

		IPaddress ipAddress;
		if (SDLNet_ResolveHost(&ipAddress, (char *) entry.published.c_str(), 0) == 0)
		{
			entry.ip = SDLNet_Read32(&ipAddress.host);
		}
		else entry.ip = 0;
		ids_.push_back(entry);
	}
	return file.getRootNode()->failChildren();
}

bool UniqueIdStore::saveStore()
{
	XMLNode idsNode("ids");
	std::list<Entry>::iterator itor;
	for (itor = ids_.begin();
		itor != ids_.end();
		++itor)
	{
		Entry *entry = &(*itor);
		XMLNode *idNode = new XMLNode("id");
		idsNode.addChild(idNode);
		idNode->addChild(
			new XMLNode("id", entry->id.c_str()));
		idNode->addChild(
			new XMLNode("published", entry->published.c_str()));
	}

	std::string idsPath = S3D::getSettingsFile("ids.xml");
	if (!idsNode.writeToFile(idsPath)) return false;
	return true;
}

const char *UniqueIdStore::getUniqueId(unsigned int ip)
{
	std::list<Entry>::iterator itor;
	for (itor = ids_.begin();
		itor != ids_.end();
		++itor)
	{
		Entry &entry = *itor;
		if (entry.ip == ip)
		{
			return entry.id.c_str();
		}
	}

	static char buffer[128];
	buffer[0] = '\0';
	return buffer;
}

bool UniqueIdStore::saveUniqueId(unsigned int ip, const char *id,
	const char *published)
{
	// No unique id
	if (0 == id[0]) return true;

	// AutoDetect server no unique id saved
	if (0 == strcmp(published, "AutoDetect")) return true;

	if (0 == strcmp(published, "scorched3d.game-host.org"))
	{
		published = "game-host.scorched3d.co.uk";
	}

	// Check the published ip matches the actual server ip
	IPaddress address;
	if (SDLNet_ResolveHost(&address, (char *) published, 0) != 0)
	{
		Logger::log(S3D::formatStringBuffer("Failed to resolve published server host \"%s\"", published));
		return false;
	}

	unsigned int ipAddress = SDLNet_Read32(&address.host);
	if (ipAddress != ip) 
	{
		std::string actualIp = NetInterface::getIpName(ip);
		std::string pubIp = NetInterface::getIpName(ipAddress);
		Logger::log(S3D::formatStringBuffer("Server ip does not match published ip\n%s != %s (%s)",
			actualIp.c_str(), published, pubIp.c_str()));

		if (OptionsDisplay::instance()->getValidateServerIp())
		{
			return false;
		}
	}

	// If it does, store this id against the published name
	bool found = false;
	std::list<Entry>::iterator itor;
	for (itor = ids_.begin();
		itor != ids_.end();
		++itor)
	{
		Entry &entry = *itor;
		if (0 == strcmp(entry.published.c_str(), published))
		{
			if (0 != strcmp(entry.id.c_str(), id))
			{
				Logger::log( "Warning: Updating to new uniqueid.");

				// Update an old id
				found = true;
				entry.id = id;
				break;
			}
			else
			{
				// Id already here and the same
				return true;
			}
		}
	}

	if (!found)
	{
		// A new id
		Entry entry;
		entry.id = id;
		entry.ip = ip;
		entry.published = published;
		ids_.push_back(entry);
	}

	// Save this id
	saveStore();
	return true;
}
