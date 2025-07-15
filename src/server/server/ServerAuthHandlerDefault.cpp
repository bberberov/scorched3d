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

#include <server/ServerAuthHandlerDefault.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>

ServerAuthHandlerDefault::ServerAuthHandlerDefault()
{
}

ServerAuthHandlerDefault::~ServerAuthHandlerDefault()
{
}

void ServerAuthHandlerDefault::createAuthentication(ComsConnectAuthMessage &authMessage)
{
	if (ScorchedServer::instance()->getOptionsGame().getServerPassword()[0])
	{
		authMessage.setPassword("required");
	}
}

bool ServerAuthHandlerDefault::authenticateUser(ComsConnectAuthMessage &authMessage, 
	std::string &message)
{
	// Check for a password (if any)
	if (ScorchedServer::instance()->getOptionsGame().getServerPassword()[0])
	{
		if (0 != strcmp(authMessage.getPassword(),
			ScorchedServer::instance()->getOptionsGame().getServerPassword()))
		{
			const char *kickMessage = 
				"--------------------------------------------------\n"
				"This server is running a password protected game.\n"
				"Your supplied password does not match.\n"
				"Connection failed.\n"
				"--------------------------------------------------";
			message = kickMessage;
			return false;
		}
	}

	return true;
}

bool ServerAuthHandlerDefault::authenticateUserName(const char *uniqueId, 
	const LangString &playername)
{
	return true;
}

void ServerAuthHandlerDefault::banUser(const char *uniqueId)
{
}
