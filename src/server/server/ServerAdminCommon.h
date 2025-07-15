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

#if !defined(__INCLUDE_ServerAdminCommonh_INCLUDE__)
#define __INCLUDE_ServerAdminCommonh_INCLUDE__

#include <common/DefinesString.h>
#include <common/ChannelText.h>
#include <server/ServerAdminSessions.h>

namespace ServerAdminCommon
{
	bool addPlayer(ServerAdminSessions::Credential &credential, const char *playerType);
	bool kickPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId);
	bool poorPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId);
	bool killPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId);
	bool banPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason);
	bool flagPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason);
	bool changeNamePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const LangString &newName);
	bool slapPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, float slap);
	bool mutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, bool mute);
	bool permMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason);
	bool unpermMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId);
	bool newGame(ServerAdminSessions::Credential &credential);
	bool killAll(ServerAdminSessions::Credential &credential);
	bool stopServer(ServerAdminSessions::Credential &credential);
	bool stopServerWhenEmpty(ServerAdminSessions::Credential &credential);
	bool setLogging(ServerAdminSessions::Credential &credential, bool logging);

	void adminLog(const ChannelText &message);
	bool adminSay(ServerAdminSessions::Credential &credential,
		const char *channel, const char *text);
}

#endif
