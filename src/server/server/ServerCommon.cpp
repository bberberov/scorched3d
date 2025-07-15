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

#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerDestinations.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/FileLogger.h>
#include <common/Defines.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsConnectRejectMessage.h>
#include <net/NetInterface.h>

static FileLogger *serverFileLogger = 0;

void ServerCommon::startFileLogger(const std::string &settingsFile)
{
	if (!serverFileLogger) 
	{
		OptionsGame optionsGame;
		optionsGame.readOptionsFromFile(settingsFile);

		char buffer[256];
		snprintf(buffer, 256, "ServerLog-%i-", optionsGame.getPortNo());

		serverFileLogger = new FileLogger(buffer);
		if (0 != strcmp(optionsGame.getServerFileLogger(), "none"))
		{
			Logger::addLogger(serverFileLogger);
			Logger::log( "Created file logger.");
		}
		else
		{
			Logger::log( "Not created file logger.");
		}
	}	
}

void ServerCommon::kickDestination(unsigned int destinationId, 
	const std::string &message)
{
	Logger::log(S3D::formatStringBuffer("Kicking destination \"%i\" %s", 
		destinationId, message.c_str()));

	bool kickedPlayers = false;
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if (tank->getDestinationId() == destinationId)
		{
			kickedPlayers = true;
			kickPlayer(tank->getPlayerId(), message);
		}
	}
	
	// Make sure we disconnect even if a player has not been created yet
	if (!kickedPlayers)
	{
		// Form the disconnect reason
		ComsConnectRejectMessage rejectMessage(message.c_str());
		NetBuffer netBuffer;
		rejectMessage.writeTypeMessage(netBuffer);
		rejectMessage.writeMessage(netBuffer);
		netBuffer.addToBuffer(false);

		// Disconnect client
		ScorchedServer::instance()->getNetInterface().
			disconnectClient(netBuffer, destinationId);
		ScorchedServer::instance()->getNetInterface().processMessages();
	}
}

void ServerCommon::kickPlayer(unsigned int playerId,
	const std::string &message)
{
	Logger::log(S3D::formatStringBuffer("Kicking player \"%i\" %s", playerId, message.c_str()));

	Tank *tank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (tank)
	{
		ScorchedServer::instance()->getServerChannelManager().sendText(ChannelText("info", 
			"ADMIN_PLAYER_KICKED", 
			"[p:{0}] has been kicked from the server",
			tank->getTargetName()), true);
		Logger::log(S3D::formatStringBuffer("Kicking client \"%s\" \"%i\"", 
			tank->getCStrName().c_str(), tank->getPlayerId()));

		// Check to see if the destination exists
		// It may not if the tank is an AI, or if something has gone wrong when creating the tank
		// at least this way we can always kick it
		ServerDestination *destination =
			ScorchedServer::instance()->getServerDestinations().getDestination(tank->getDestinationId());
		if (!destination)
		{
			ScorchedServer::instance()->getServerMessageHandler().
				destroyPlayer(tank->getPlayerId(), message.c_str());
		}
		else
		{
			// Form the disconnect reason
			ComsConnectRejectMessage rejectMessage(message.c_str());
			NetBuffer netBuffer;
			rejectMessage.writeTypeMessage(netBuffer);
			rejectMessage.writeMessage(netBuffer);
			netBuffer.addToBuffer(false);

			// Disconnect Client
			ScorchedServer::instance()->getNetInterface().
				disconnectClient(netBuffer, tank->getDestinationId());
			ScorchedServer::instance()->getNetInterface().processMessages();
		}
	}
}

bool &ServerCommon::getExitEmpty()
{
	static bool exitEmpty = false;
	return exitEmpty;
}

void ServerCommon::serverLog(const std::string &text)
{
#ifdef S3D_SERVER
	{
		Logger::log(text);
	}
#endif
}
