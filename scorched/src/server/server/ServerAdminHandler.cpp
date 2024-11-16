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

#include <server/ServerAdminHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerChannelManager.h>
#include <server/ServerCommon.h>
#include <server/ServerSyncCheck.h>
#include <server/ServerAdminCommon.h>
#include <server/ServerAdminSessions.h>
#include <server/ServerDestinations.h>
#include <server/ServerBanned.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <coms/ComsAdminMessage.h>
#include <coms/ComsAdminResultMessage.h>
#include <coms/ComsMessageSender.h>
#include <net/NetInterface.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <stdlib.h>

ServerAdminHandler::ServerAdminHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsAdminMessage::ComsAdminMessageTyper,
		this);
}

ServerAdminHandler::~ServerAdminHandler()
{
}

bool ServerAdminHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsAdminMessage message;
	if (!message.readMessage(reader)) return false;

	unsigned int destinationId = netMessage.getDestinationId();

	ServerDestination *destinationInfo =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (!destinationInfo) return false;

	// Check if the SID is valid
	ServerAdminSessions::SessionParams *adminSession =
		ScorchedServer::instance()->getServerAdminSessions().getSession(message.getSid());

	// Check if we are logging in
	if (message.getType() == ComsAdminMessage::AdminLogin ||
		message.getType() == ComsAdminMessage::AdminLoginLocal)
	{	
		unsigned int sid = message.getSid();
		if (adminSession ||
			(sid = ScorchedServer::instance()->getServerAdminSessions().
				login(message.getParam1(), message.getParam2(),
				NetInterface::getIpName(netMessage.getIpAddress()))) != 0)
		{
			adminSession =
				ScorchedServer::instance()->getServerAdminSessions().getSession(sid);

			ScorchedServer::instance()->getServerChannelManager().refreshDestination(destinationId);

			ScorchedServer::instance()->getServerChannelManager().sendText(
				ChannelText("info", "ADMIN_LOGGED_IN", 
					"Server admin \"{0}\" logged in",
					adminSession->credentials.username),
				true);
			ServerCommon::serverLog(
				S3D::formatStringBuffer(
					"Server admin \"%s\" logged in, destination id \"%u\"",
				adminSession->credentials.username.c_str(),
				destinationId));

			ComsAdminResultMessage resultMessage(sid, message.getType());
			ComsMessageSender::sendToSingleClient(resultMessage, destinationId);
			destinationInfo->setAdmin(true);
			destinationInfo->setAdminTries(0);

			return true;
		}
		else
		{
			if (message.getType() != ComsAdminMessage::AdminLoginLocal)
			{
				destinationInfo->setAdminTries(
					destinationInfo->getAdminTries() + 1);
				
				ScorchedServer::instance()->getServerChannelManager().sendText(
					ChannelText("info", 
						"INCORRECT_PASSWORD",
						"Incorrect admin password (try {0}/3)", 
						destinationInfo->getAdminTries()),
					destinationId,
					true);
				if (destinationInfo->getAdminTries() > 3)
				{
					ServerCommon::kickDestination(destinationId,
						"Due to incorrect admin passwords");
				}

				Logger::log(S3D::formatStringBuffer(
					"Failed login for server admin \"%s\" via console, ip \"%s\", destination id \"%u\"",
					message.getParam1(),
					NetInterface::getIpName(netMessage.getIpAddress()),
					destinationId));
			}

			ComsAdminResultMessage resultMessage(0, message.getType());
			ComsMessageSender::sendToSingleClient(resultMessage, destinationId);
			destinationInfo->setAdmin(false);

			return true;
		}
	}

	if (!adminSession)
	{
		ScorchedServer::instance()->getServerChannelManager().sendText(
			ChannelText("info", "ADMIN_NOT_LOGGED_IN", 
				"You are not logged in as admin"),
			destinationId,
			false);

		ComsAdminResultMessage resultMessage(0, message.getType());
		ComsMessageSender::sendToSingleClient(resultMessage, destinationId);
		destinationInfo->setAdmin(false);

		return true;	
	}
	const char *adminName = adminSession->credentials.username.c_str();

	// Do admin fn (we are logged in at this point)
	switch (message.getType())
	{
	case ComsAdminMessage::AdminShow:
		{
			std::map<unsigned int, Tank *> &tanks = 
				ScorchedServer::instance()->getTargetContainer().getTanks();
			std::string result;
			result += 
				"--Admin Show-----------------------------------------\n";
			std::map<unsigned int, Tank *>::iterator itor;
			for (itor = tanks.begin();
				itor != tanks.end();
				++itor)
			{
				Tank *tank = (*itor).second;

				fixed ping = 0;
				ServerDestination *destination = 
					ScorchedServer::instance()->getServerDestinations().
					getDestination(tank->getDestinationId());
				if (destination) ping = destination->getPing().getAverage();

				result += 
					S3D::formatStringBuffer("%u \"%s\" \"%s\" %s Ping %i\n",
						tank->getPlayerId(), 
						tank->getCStrName().c_str(),
						NetInterface::getIpName(tank->getIpAddress()),
						(tank->getState().getMuted()?"Muted":"Not Muted"),
						(ping * 1000).asInt());
			}
			result +=
				"-----------------------------------------------------\n";

			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("info", LANG_STRING(result)),
				destinationId,
				false);
		}
		break;
	case ComsAdminMessage::AdminLogout:
		{
			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("info", "ADMIN_LOGGED_OUT", 
					"Server admin \"{0}\" logged out",
					adminName),
				true);
			ServerCommon::serverLog(
				S3D::formatStringBuffer("Server admin \"%s\" logged out",
				adminName));

			ScorchedServer::instance()->getServerAdminSessions().logout(message.getSid());
			ScorchedServer::instance()->getServerChannelManager().refreshDestination(destinationId);

			ComsAdminResultMessage resultMessage(0, message.getType());
			ComsMessageSender::sendToSingleClient(resultMessage, destinationId);
			destinationInfo->setAdmin(false);
		}
		break;
	case ComsAdminMessage::AdminShowBanned:
		{
			std::string result;
			result += 
				"--Admin Show Banned----------------------------------\n";

			std::list<ServerBanned::BannedRange> &bannedIps = 
				ScorchedServer::instance()->getBannedPlayers().getBannedIps();
			std::list<ServerBanned::BannedRange>::iterator itor;
			for (itor = bannedIps.begin();
				itor != bannedIps.end();
				++itor)
			{
				ServerBanned::BannedRange &range = (*itor);
				std::string mask = NetInterface::getIpName(range.mask);

				std::map<unsigned int, ServerBanned::BannedEntry>::iterator ipitor;
				for (ipitor = range.ips.begin();
					ipitor != range.ips.end();
					++ipitor)
				{
					unsigned int ip = (*ipitor).first;
					ServerBanned::BannedEntry &entry = (*ipitor).second;
					std::string ipName = NetInterface::getIpName(ip);
					std::string name = LangStringUtil::convertFromLang(entry.name);

					result += S3D::formatStringBuffer("\"%s:%s:%s\" %s %s (%s) - %s",
						name.c_str(),
						entry.uniqueid.c_str(),
						entry.SUI.c_str(),
						ServerBanned::getBannedTypeStr(entry.type),
						ipName.c_str(), mask.c_str(),
						(entry.bantime?ctime(&entry.bantime):"\n"));
				}
			}
			result +=
				"-----------------------------------------------------\n";

			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("info", LANG_STRING(result)),
				destinationId, 
				false);
		}
		break;
	case ComsAdminMessage::AdminBan:
		{
			if (!ServerAdminCommon::banPlayer(
				adminSession->credentials,
				atoi(message.getParam1()), "<via console>"))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info", 
						"UNKNOWN_PLAYER_BAN", 
						"Unknown player for ban"),
					destinationId,
					false);
			}
		}
		break;
	case ComsAdminMessage::AdminFlag:
		{
			if (!ServerAdminCommon::flagPlayer(
				adminSession->credentials,
				atoi(message.getParam1()), "<via console>"))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_FLAG",
						"Unknown player for flag"),
					destinationId,
					false);
			}
		}
		break;
	case ComsAdminMessage::AdminPoor:
		{
			if (!ServerAdminCommon::poorPlayer(
				adminSession->credentials,
				atoi(message.getParam1())))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_POOR",
						"Unknown player for poor"),
					destinationId,
					false);
			}
		}
		break;	
	case ComsAdminMessage::AdminKick:
		{
			if (!ServerAdminCommon::kickPlayer(
				adminSession->credentials,
				atoi(message.getParam1())))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info", 
						"UNKNOWN_PLAYER_KICK",
						"Unknown player for kick"),
					destinationId,
					false);
			}
		}
		break;
	case ComsAdminMessage::AdminMute:
	case ComsAdminMessage::AdminUnMute:
		{
			bool mute = (message.getType() == ComsAdminMessage::AdminMute);
			if (!ServerAdminCommon::mutePlayer(
				adminSession->credentials,
				atoi(message.getParam1()), mute))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_MUTE",
						"Unknown player for mute"),
					destinationId,
					false);
			}	
		}
		break;
	case ComsAdminMessage::AdminPermMute:
		{
			if (!ServerAdminCommon::permMutePlayer(
				adminSession->credentials,
				atoi(message.getParam1()), "<via console>"))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_PERMMUTE",
						"Unknown player for permmute"),
					destinationId,
					false);
			}	
		}
		break;
	case ComsAdminMessage::AdminUnPermMute:
		{
			if (!ServerAdminCommon::unpermMutePlayer(
				adminSession->credentials,
				atoi(message.getParam1())))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_UNPERMMUTE",
						"Unknown player for unpermmute"),
					destinationId,
					false);
			}
		}
		break;
	case ComsAdminMessage::AdminTalk:
		ServerAdminCommon::adminSay(adminSession->credentials, "info", message.getParam1());
		break;
	case ComsAdminMessage::AdminAdminTalk:
		ServerAdminCommon::adminSay(adminSession->credentials, "admin", message.getParam1());
		break;
	case ComsAdminMessage::AdminMessage:
		ServerAdminCommon::adminSay(adminSession->credentials, "banner", message.getParam1());
		break;
	case ComsAdminMessage::AdminSyncCheck:
		{
			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("info", "SENDING_SYNC", "sending sync..."),
				destinationId, true);
			ScorchedServer::instance()->getServerSyncCheck().sendSyncCheck();
		}
		break;
	case ComsAdminMessage::AdminKillAll:
		ServerAdminCommon::killAll(adminSession->credentials);
		break;
	case ComsAdminMessage::AdminNewGame:
		ServerAdminCommon::newGame(adminSession->credentials);
		break;	
	case ComsAdminMessage::AdminSlap:
		{
			if (!ServerAdminCommon::slapPlayer(
				adminSession->credentials,
				atoi(message.getParam1()), (float) atof(message.getParam2())))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info", 
						"UNKNOWN_PLAYER_SLAP", 
						"Unknown player for slap"),
					destinationId, false);
			}
		}
		break;
	case ComsAdminMessage::AdminChangeName:
		{
			if (!ServerAdminCommon::changeNamePlayer(
				adminSession->credentials,
				atoi(message.getParam1()), 
				LangStringUtil::convertToLang(message.getParam2())))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info", 
						"UNKNOWN_PLAYER_CHANGE_NAME", 
						"Unknown player for change name"),
					destinationId, false);
			}
		}
		break;
	case ComsAdminMessage::AdminAdd:
		{
			if (!ServerAdminCommon::addPlayer(
				adminSession->credentials,
				message.getParam1()))
			{
				ScorchedServer::instance()->getServerChannelManager().sendText( 
					ChannelText("info",
						"UNKNOWN_PLAYER_ADD",
						"Unknown player type to add"),
					destinationId, false);
			}	
		}
		break;
	}

	return true;
}
