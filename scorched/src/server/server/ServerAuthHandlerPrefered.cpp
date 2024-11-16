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

#include <server/ServerAuthHandlerPrefered.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <XML/XMLFile.h>

ServerAuthHandlerPrefered::ServerAuthHandlerPrefered() : lastReadTime_(0)
{
}

ServerAuthHandlerPrefered::~ServerAuthHandlerPrefered()
{
}

void ServerAuthHandlerPrefered::createAuthentication(ComsConnectAuthMessage &authMessage)
{
}

bool ServerAuthHandlerPrefered::authenticateUser(ComsConnectAuthMessage &authMessage, 
		std::string &message)
{
	if (!getUserById(authMessage.getUniqueId()))
	{
		message = 
			"This server is running a prefered player only game.\n"
			"Your supplied unique id is not in the prefered player list.\n";
		return false;
	}

	return true;
}

bool ServerAuthHandlerPrefered::authenticateUserName(const char *uniqueId, 
	const LangString &playername)
{
	std::string strName = LangStringUtil::convertFromLang(playername);
	UserEntry *userEntry = getUserByName(strName.c_str());
	if (!userEntry) return true;
	if (0 == strcmp(userEntry->uniqueid.c_str(), uniqueId)) return true;
	return false;
}

void ServerAuthHandlerPrefered::banUser(const char *uniqueId)
{
}

ServerAuthHandlerPrefered::UserEntry *ServerAuthHandlerPrefered::getUserByName(const char *name)
{
	load();

	std::list<UserEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		UserEntry &entry = *itor;
		if (0 == strcmp(entry.name.c_str(), name))
		{
			return &entry;
		}
	}
	return 0;
}

ServerAuthHandlerPrefered::UserEntry *ServerAuthHandlerPrefered::getUserById(const char *uniqueId)
{
	load();

	std::list<UserEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		UserEntry &entry = *itor;
		if (0 == strcmp(entry.uniqueid.c_str(), uniqueId))
		{
			return &entry;
		}
	}
	return 0;
}

bool ServerAuthHandlerPrefered::load()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("preferedplayers-%i.xml", 
			ScorchedServer::instance()->getOptionsGame().getPortNo()));
	if (!::S3D::fileExists(filename)) return true;

	time_t fileTime = S3D::fileModTime(filename);
	if (fileTime == lastReadTime_) return true;

	XMLFile file;
	if (!file.readFile(filename))
	{
		Logger::log(S3D::formatStringBuffer("Failed to parse user file \"%s\"\n%s", 
			filename.c_str(), file.getParserError()));
		return false;
	}

	Logger::log(S3D::formatStringBuffer("Refreshing user list %s", filename.c_str()));
	lastReadTime_ = (unsigned int) fileTime;
	entries_.clear();
	if (!file.getRootNode()) return true; // Empty File

	std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
	for (childrenItor = children.begin();
		 childrenItor != children.end();
		++childrenItor)
	{
		XMLNode *currentNode = (*childrenItor);

		UserEntry entry;
		if (!currentNode->getNamedChild("name", entry.name)) return false;
		if (!currentNode->getNamedChild("uniqueid", entry.uniqueid)) return false;
		if (!currentNode->failChildren()) return false;
		entries_.push_back(entry);
	}
	return true;
}
