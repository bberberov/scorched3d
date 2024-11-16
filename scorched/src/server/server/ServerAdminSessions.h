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

#if !defined(__INCLUDE_ServerAdminSessionsh_INCLUDE__)
#define __INCLUDE_ServerAdminSessionsh_INCLUDE__

#include <map>
#include <set>
#include <list>
#include <string>

class ServerAdminSessions
{
public:
	ServerAdminSessions();
	virtual ~ServerAdminSessions();

	static std::string PERMISSION_BANPLAYER;
	static std::string PERMISSION_KICKPLAYER;
	static std::string PERMISSION_ALIASPLAYER;
	static std::string PERMISSION_ADDPLAYER;
	static std::string PERMISSION_VIEWLOGS;
	static std::string PERMISSION_ALTERGAME;
	static std::string PERMISSION_ALTERSERVER;
	static std::string PERMISSION_ALTERSETTINGS;

	// Creds
	struct Credential
	{
		std::string username;
		std::string password;
		std::set<std::string> permissions;

		bool hasPermission(const std::string &perm) 
			{ return (permissions.find(perm) != permissions.end()); }
	};

	// Sessions
	struct SessionParams
	{
		unsigned int sessionTime;
		unsigned int sid;
		std::string ipAddress;
		Credential credentials;
	};

	unsigned int login(const char *name, const char *password, const char *ipAddress);
	void logout(unsigned int sid);

	SessionParams *getFirstSession();
	SessionParams *getSession(unsigned int sid);

	bool setPassword(const char *name, 
		const char *oldpassword, const char *newpassword);

	Credential &getLocalUserCredentials() { return localCreds_; }
	bool getAllCredentials(std::list<Credential> &creds);
	bool setAllCredentials(std::list<Credential> &creds);
	std::map<unsigned int, SessionParams> &getAllSessions() { return sessions_; }

protected:
	Credential localCreds_;
	std::map<unsigned int, SessionParams> sessions_;

};

#endif
