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

#if !defined(__INCLUDE_ServerAuthHandlerForumLoginh_INCLUDE__)
#define __INCLUDE_ServerAuthHandlerForumLoginh_INCLUDE__

#ifdef HAVE_MYSQL

#if defined(_WIN32)
#include <Winsock2.h>
#endif
#include <server/ServerAuthHandler.h>
#include <mysql/mysql.h>

class ServerAuthHandlerForumLogin : public ServerAuthHandler
{
public:
	ServerAuthHandlerForumLogin();
	virtual ~ServerAuthHandlerForumLogin();

	virtual void createAuthentication(ComsConnectAuthMessage &authMessage);
	virtual bool authenticateUser(ComsConnectAuthMessage &authMessage, 
		std::string &message);
	virtual bool authenticateUserName(const char *uniqueId, 
		const LangString &playername);
	virtual void banUser(const char *uniqueId);

protected:
	MYSQL *mysql_;
	std::string name_;
	bool success_;

	bool connectHandler();
};

#endif // HAVE_MYSQL

#endif // __INCLUDE_ServerAuthHandlerForumLoginh_INCLUDE__
