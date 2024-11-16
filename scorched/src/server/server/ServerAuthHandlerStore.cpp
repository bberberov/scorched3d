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

#include <server/ServerAuthHandlerStore.h>
#include <server/ServerAuthHandlerForumLogin.h>
#include <server/ServerAuthHandlerPrefered.h>
#include <server/ServerAuthHandlerMinKills.h>
#include <server/ServerAuthHandlerDefault.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>

ServerAuthHandlerStore::ServerAuthHandlerStore()
{
}

ServerAuthHandlerStore::~ServerAuthHandlerStore()
{
}

ServerAuthHandler *ServerAuthHandlerStore::getAuthHandler()
{
	const char *handler = ScorchedServer::instance()->getOptionsGame().getAuthHandler();
	if (0 == strcmp("none", handler))
	{
		static ServerAuthHandlerDefault *authHandler = new ServerAuthHandlerDefault();
		return authHandler;
	} 
	else if (0 == strcmp("prefered", handler))
	{
		static ServerAuthHandlerPrefered *authHandler = new ServerAuthHandlerPrefered();
		return authHandler;
	}
#ifdef HAVE_MYSQL
	else if (0 == strcmp("forumlogin", handler))
	{
		static ServerAuthHandlerForumLogin *authHandler = new ServerAuthHandlerForumLogin();
		return authHandler;
	}
#endif
	else if (0 == strcmp("minkills", handler))
	{
		static ServerAuthHandlerMinKills *authHandler = new ServerAuthHandlerMinKills();
		return authHandler;
	}
	else 
	{
		S3D::dialogExit("ServerAuthHandler", 
			S3D::formatStringBuffer("Unknown auth handler \"%s\"", handler));
	}
	return 0;
}