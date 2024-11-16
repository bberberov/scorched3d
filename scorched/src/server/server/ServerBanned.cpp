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

#include <server/ServerBanned.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <net/NetInterface.h>
#include <XML/XMLFile.h>
#include <limits.h>

ServerBanned::ServerBanned() : lastReadTime_(0)
{
}

ServerBanned::~ServerBanned()
{
}

bool ServerBanned::load(bool force)
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("banned-%i.xml", 
			ScorchedServer::instance()->getOptionsGame().getPortNo()));
	if (!S3D::fileExists(filename)) return true;

	time_t fileTime = S3D::fileModTime(filename);
	if (!force && fileTime == lastReadTime_) return true;

	XMLFile file;
	if (!file.readFile(filename))
	{
		Logger::log(S3D::formatStringBuffer("Failed to parse banned file \"%s\"\n%s", 
			filename.c_str(), file.getParserError()));
		return false;
	}

	Logger::log(S3D::formatStringBuffer("Refreshing banned list %s", filename.c_str()));
	lastReadTime_ = fileTime;
	bannedIps_.clear();
	bannedIds_.clear();
	bannedSUIs_.clear();
	if (!file.getRootNode()) return true; // Empty File

	std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
	for (childrenItor = children.begin();
		 childrenItor != children.end();
		++childrenItor)
	{
		XMLNode *currentNode = (*childrenItor);
		XMLNode *maskNode = 0, *timeNode = 0, *typeNode = 0;

		// Read the mask
		unsigned int m = UINT_MAX;
		if (currentNode->getNamedParameter("mask", maskNode, false))
		{
			unsigned int mask[4];
			if (sscanf(maskNode->getContent(), "%u.%u.%u.%u", 
				&mask[3], &mask[2], &mask[1], &mask[0]) != 4)
			{
				S3D::dialogMessage("ServerBanned",
					S3D::formatStringBuffer("Failed to parse mask %s",
					maskNode->getContent()));
				return false;
			}
			m = mask[3] << 24 | mask[2] << 16 | mask[1] << 8 | mask[0];
		}

		LangString name;
		std::string adminname, uniqueid, reason, SUI;
		currentNode->getNamedParameter("name", name, false);
		currentNode->getNamedParameter("adminname", adminname, false);
		currentNode->getNamedParameter("reason", reason, false);
		currentNode->getNamedParameter("id", uniqueid, false);
		currentNode->getNamedParameter("SUI", SUI, false);

		// Time
		time_t bantime = 0;
		if (currentNode->getNamedParameter("time", timeNode, false))
		{
			sscanf(timeNode->getContent(), "%u", &bantime);
		}

		// Type
		BannedType type = Banned;
		if (currentNode->getNamedParameter("type", typeNode, false))
		{
			if (0 == strcmp("banned", typeNode->getContent())) type = Banned;
			else if (0 == strcmp("muted", typeNode->getContent())) type = Muted;
			else if (0 == strcmp("flagged", typeNode->getContent())) type = Flagged;
			else
			{
				S3D::dialogMessage("ServerBanned", 
					S3D::formatStringBuffer("Failed to parse banned type %s",
					typeNode->getContent()));
				return false;
			}
		}

		// Read the ip address
		unsigned int address[4];
		if (sscanf(currentNode->getContent(), "%u.%u.%u.%u", 
			&address[3], &address[2], &address[1], &address[0]) != 4)
		{
			S3D::dialogMessage("ServerBanned", 
				S3D::formatStringBuffer("Failed to parse ip address %s", 
				currentNode->getContent()));
			return false;
		}

		unsigned int ip = 0;
		ip = address[3] << 24 | address[2] << 16 | address[1] << 8 | address[0];

		// Add the new entry
		addBannedEntry(ip, m, name, uniqueid.c_str(), SUI.c_str(), 
			(unsigned int) bantime, type,
			adminname.c_str(), reason.c_str());
	}
	return true;
}

std::list<ServerBanned::BannedRange> &ServerBanned::getBannedIps()
{ 
	load();
	return bannedIps_; 
}

ServerBanned::BannedType ServerBanned::getBanned(
	const char *unqiueid, const char *SUI)
{
	load();

	// Check if the unique id has been banned
	std::map<std::string, BannedEntry>::iterator findItor =
		bannedIds_.find(unqiueid);
	if (findItor != bannedIds_.end())
	{
		BannedEntry &entry = (*findItor).second;
		return entry.type;
	}

	// Check if the SUI has been banned
	std::map<std::string, BannedEntry>::iterator findItor2 = 
		bannedSUIs_.find(SUI);
	if (findItor2 != bannedSUIs_.end())
	{
		BannedEntry &entry = (*findItor2).second;
		return entry.type;
	}
	return NotBanned;
}

ServerBanned::BannedType ServerBanned::getBanned(
	unsigned int ip)
{
	load();

	// Check if the ip address has been banned
	std::list<BannedRange>::iterator itor;
	for (itor = bannedIps_.begin();
		itor != bannedIps_.end();
		++itor)
	{
		BannedRange &range = *itor;
		unsigned int newip = range.mask & ip;
		std::map<unsigned int, BannedEntry>::iterator findItor =
			range.ips.find(newip);
		if (findItor != range.ips.end())
		{
			BannedEntry &entry = (*findItor).second;
			return entry.type;
		}
	}
	return NotBanned;
}

void ServerBanned::addBanned(unsigned int ip, const LangString &name, 
	const char *uniqueId, const char *SUI, BannedType type, 
	const char *adminname, const char *reason)
{
	unsigned int t = (unsigned int) time(0);
	addBannedEntry(ip, UINT_MAX, name, uniqueId, SUI, t, type, adminname, reason);
	save();
}

void ServerBanned::addBannedEntry(unsigned int ip, unsigned int mask,
	const LangString &name, const char *uniqueId, const char *SUid, unsigned int bantime, BannedType type,
	const char *adminname, const char *reason)
{
	unsigned int newip = mask & ip;
	BannedEntry newEntry;
	newEntry.name = name;
	newEntry.bantime = bantime;
	newEntry.type = type;
	newEntry.uniqueid = uniqueId;
	newEntry.SUI = SUid;
	newEntry.adminname = adminname;
	newEntry.reason = reason;

	BannedRange *found = 0;
	std::list<BannedRange>::iterator itor;
	for (itor = bannedIps_.begin();
		itor != bannedIps_.end();
		++itor)
	{
		BannedRange &range = *itor;
		if (range.mask == mask)
		{
			found = &range;
			break;
		}
	}
	if (!found)
	{
		BannedRange range;
		range.mask = mask;
		bannedIps_.push_back(range);
		found = &bannedIps_.back();
	}

	// Add ip to list of banned ips
	found->ips[newip] = newEntry;

	// Add id to list of banned ids
	if (uniqueId[0])
	{
		std::map<std::string, BannedEntry>::iterator findItor =
			bannedIds_.find(uniqueId);
		if (findItor == bannedIds_.end())
		{
			bannedIds_[uniqueId] = newEntry;
		}
	}

 	// Add SUI to list of banned SUIs
 	if (SUid[0])
 	{
 		std::map<std::string, BannedEntry>::iterator findItor2 = 
			bannedSUIs_.find(SUid);
 		if (findItor2 == bannedSUIs_.end())
 		{
 			bannedSUIs_[SUid] = newEntry;
 		} 		
 	}
}

const char *ServerBanned::getBannedTypeStr(BannedType type)
{
	const char *str = "error";
	switch (type)
	{
		case Muted:
			str = "muted";
			break;
		case Banned:
			str = "banned";
			break;
		case NotBanned:
			str = "notbanned";
			break;	
		case Flagged:
			str = "flagged";
			break;		
	}
	return str;
}

bool ServerBanned::save()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("banned-%i.xml", 
			ScorchedServer::instance()->getOptionsGame().getPortNo()));

	XMLNode node("bannednodes");
	std::list<BannedRange>::iterator itor;
	for (itor = bannedIps_.begin();
		itor != bannedIps_.end();
		++itor)
	{
		BannedRange &range = *itor;
		unsigned int m = range.mask;
		std::map<unsigned int, BannedEntry>::iterator ipitor;
		for (ipitor = range.ips.begin();
			ipitor != range.ips.end();
			++ipitor)
		{
			// Add ip address
			unsigned int ip = (*ipitor).first;
			BannedEntry &entry = (*ipitor).second;
			XMLNode *optionNode = 
				new XMLNode("ipaddress", NetInterface::getIpName(ip));

			// Add the mask
			if (m != UINT_MAX)
			{
				optionNode->addParameter(new XMLNode("mask", NetInterface::getIpName(m),
						XMLNode::XMLParameterType));
			}
			optionNode->addParameter(new XMLNode("name", 
					entry.name,
					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("time", 
					S3D::formatStringBuffer("%u", (unsigned int) entry.bantime),
					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("type", 
					getBannedTypeStr(entry.type),
					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("id", 
					entry.uniqueid,
					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("SUI",
 					entry.SUI,
 					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("adminname", 
					entry.adminname,
					XMLNode::XMLParameterType));
			optionNode->addParameter(new XMLNode("reason", 
					entry.reason,
					XMLNode::XMLParameterType));

			// Add to file
			if (entry.type != NotBanned)
			{
				node.addChild(optionNode);
			}
		}
	}
	return node.writeToFile(filename);
}

