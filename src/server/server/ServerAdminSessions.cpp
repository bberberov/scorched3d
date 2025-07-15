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

#include <server/ServerAdminSessions.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <XML/XMLFile.h>
#include <time.h>

std::string ServerAdminSessions::PERMISSION_BANPLAYER("banplayer");
std::string ServerAdminSessions::PERMISSION_KICKPLAYER("kickplayer");
std::string ServerAdminSessions::PERMISSION_ALIASPLAYER("aliasplayer");
std::string ServerAdminSessions::PERMISSION_ADDPLAYER("addplayer");
std::string ServerAdminSessions::PERMISSION_VIEWLOGS("viewlogs");
std::string ServerAdminSessions::PERMISSION_ALTERGAME("altergame");
std::string ServerAdminSessions::PERMISSION_ALTERSERVER("alterserver");
std::string ServerAdminSessions::PERMISSION_ALTERSETTINGS("altersettings");

ServerAdminSessions::ServerAdminSessions()
{
	localCreds_.password = "";
	localCreds_.username = "localaccount";
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_BANPLAYER);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_KICKPLAYER);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_ALIASPLAYER);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_ADDPLAYER);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_VIEWLOGS);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_ALTERGAME);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_ALTERSERVER);
	localCreds_.permissions.insert(ServerAdminSessions::PERMISSION_ALTERSETTINGS);
}

ServerAdminSessions::~ServerAdminSessions()
{
}

ServerAdminSessions::SessionParams *ServerAdminSessions::getFirstSession()
{
	if (sessions_.empty()) return 0;
	return &(sessions_.begin()->second);
}

ServerAdminSessions::SessionParams *ServerAdminSessions::getSession(unsigned int sid)
{
	const unsigned int SessionTimeOut = 60 * 15;
	unsigned int currentTime = (unsigned int) time(0);

	// Tidy expired sessions
	std::map <unsigned int, SessionParams>::iterator sitor;
	for (sitor = sessions_.begin();
		sitor != sessions_.end();
		++sitor)
	{
		SessionParams &params = (*sitor).second;
		if (currentTime > params.sessionTime + SessionTimeOut)
		{
			sessions_.erase(sitor);
			break;
		}
	}

	// Check if user has a valid session
	std::map <unsigned int, SessionParams>::iterator findItor =
		sessions_.find(sid);
	if (findItor != sessions_.end())
	{
		SessionParams &params = (*findItor).second;
		if (currentTime < params.sessionTime + SessionTimeOut)
		{
			params.sessionTime = currentTime;
			return &params;
		}
		else
		{
			sessions_.erase(findItor);
		}
	}

	return 0;
}

unsigned int ServerAdminSessions::login(const char *name, const char *password, const char *ipAddress)
{
	bool local = (0 == strcmp(ipAddress, "127.0.0.1"));
#ifndef S3D_SERVER
	local = true;
#endif

	// Find if the username and password match
	Credential userCreds;
	if (local)
	{
		userCreds = localCreds_;
	}
	else
	{
		bool found = false;
		{
			std::list<Credential> creds;
			getAllCredentials(creds);
			std::list<Credential>::iterator itor;
			for (itor = creds.begin();
				itor != creds.end();
				++itor)
			{
				Credential &credential = (*itor);
				if (0 == strcmp(name, credential.username.c_str()) &&
					0 == strcmp(password, credential.password.c_str()))
				{
					userCreds = credential;
					found = true;
					break;
				}
			}
		}
		if (!found) return 0;
	}

	unsigned int sid = 0;

	// Try to find an existing session for this user
	std::map<unsigned int, SessionParams>::iterator itor;
	for (itor = sessions_.begin();
		itor != sessions_.end();
		++itor)
	{
		SessionParams &params = (*itor).second;
		if (0 == strcmp(params.credentials.username.c_str(), 
			userCreds.username.c_str()))
		{
			// Found one
			sid = (*itor).first;
			break;
		}
	}

	// Generate a sid if we didn't find an existing login
	if (sid == 0)
	{
		// Generate a new unique session id
		do 
		{
			sid = rand();
		} while (sessions_.find(sid) != sessions_.end() || sid == 0);
	}

	// Update the session params
	unsigned int currentTime = (unsigned int) time(0);
	SessionParams params;
	params.sessionTime = currentTime;
	params.credentials = userCreds;
	params.ipAddress = ipAddress;
	params.sid = sid;
	sessions_[sid] = params;

	return sid;
}

void ServerAdminSessions::logout(unsigned int sid)
{
	std::map <unsigned int, SessionParams>::iterator sitor =
		sessions_.find(sid);
	if (sitor != sessions_.end())
	{
		sessions_.erase(sitor);
	}
}

bool ServerAdminSessions::setPassword(const char *name, 
	const char *oldpassword, const char *newpassword)
{
	std::list<Credential> creds;
	getAllCredentials(creds);
	std::list<Credential>::iterator itor;
	for (itor = creds.begin();
		itor != creds.end();
		++itor)
	{
		Credential &credential = (*itor);
		if (0 == strcmp(name, credential.username.c_str()) &&
			0 == strcmp(oldpassword, credential.password.c_str()))
		{
			credential.password = newpassword;
			setAllCredentials(creds);
			return true;
		}
	}
	return false;
}

bool ServerAdminSessions::setAllCredentials(std::list<Credential> &creds)
{
	std::string fileName = 
		S3D::getSettingsFile(S3D::formatStringBuffer("adminpassword-%i.xml",
			ScorchedServer::instance()->getOptionsGame().getPortNo()));

	XMLNode usersNode("users");
	std::list<Credential>::iterator itor;
	for (itor = creds.begin();
		itor != creds.end();
		++itor)
	{
		Credential &credential = *itor;
		XMLNode *userNode = new XMLNode("user");

		userNode->addChild(new XMLNode("name", credential.username));
		userNode->addChild(new XMLNode("password", credential.password));

		std::set<std::string>::iterator permitor;
		for (permitor = credential.permissions.begin();
			permitor != credential.permissions.end();
			++permitor)
		{
			userNode->addChild(new XMLNode("permission", *permitor));
		}

		usersNode.addChild(userNode);
	}

	if (!usersNode.writeToFile(fileName)) return false;

	return true;	
}

bool ServerAdminSessions::getAllCredentials(std::list<Credential> &creds)
{
	std::string fileName = 
		S3D::getSettingsFile(S3D::formatStringBuffer("adminpassword-%i.xml",
			ScorchedServer::instance()->getOptionsGame().getPortNo()));

	XMLFile file;
	if (!file.readFile(fileName))
	{
		ServerCommon::serverLog( 
			S3D::formatStringBuffer("Failed to parse \"%s\"\n%s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		ServerCommon::serverLog( 
			S3D::formatStringBuffer("Please create file %s to have admin users", 
			fileName.c_str()));
		return false;
	}

	// Itterate all of the users in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 ++childrenItor)
    {
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "user")) return false;

		Credential credential;
		if (!currentNode->getNamedChild("name", credential.username)) return false;
		if (!currentNode->getNamedChild("password", credential.password)) return false;

		std::string permission;
		while (currentNode->getNamedChild("permission", permission, false))
		{
			if (localCreds_.hasPermission(permission))
			{
				credential.permissions.insert(permission);
			}
			else
			{
				Logger::log(S3D::formatStringBuffer(
					"ERROR: Trying to give admin %s unavailable permission %s",
					credential.username.c_str(),
					permission.c_str()));
			}
		}

		if (!currentNode->failChildren()) return false;
		creds.push_back(credential);
	}

	return true;	
}
