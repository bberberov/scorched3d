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

#include <server/ServerConnectAuthHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerBanned.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerDestinations.h>
#include <server/ServerSimulator.h>
#include <server/ServerAuthHandler.h>
#include <server/ServerState.h>
#include <engine/SaveGame.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <target/TargetContainer.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStrings.h>
#include <tankai/TankAIStore.h>
#include <common/Defines.h>
#include <common/FileLines.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <events/EventHandlerDataBase.h>
#include <net/NetInterface.h>
#include <simactions/TankAddSimAction.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>
#ifndef S3D_SERVER
#include <client/ClientParams.h>
#endif

ServerConnectAuthHandler::ServerConnectAuthHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsConnectAuthMessage::ComsConnectAuthMessageType,
		this);
}

ServerConnectAuthHandler::~ServerConnectAuthHandler()
{
	std::list<AuthMessage *>::iterator itor;
	for (itor = authMessages_.begin();
		itor != authMessages_.end();
		++itor)
	{
		delete *itor;
	}
	authMessages_.clear();
}

bool ServerConnectAuthHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	// Decode the auth connect message
	AuthMessage *authMessage = new AuthMessage();
	authMessage->destinationId = netMessage.getDestinationId();
	authMessage->ipAddress = netMessage.getIpAddress();
	if (!authMessage->message.readMessage(reader))
	{
		ServerCommon::kickDestination(netMessage.getDestinationId(),
			"Invalid auth message format");
		delete authMessage;
		return true;
	}

	// Record the auth message till later
	authMessages_.push_back(authMessage);
	return true;
}

void ServerConnectAuthHandler::processMessages()
{
	// Only process the new auth requests once there are no more outstanding
	// tank addition requests
	// Just makes some of the checks easier
	while (!authMessages_.empty())
	{
		// Check that there are no outstanding tank addition requests
		if (TankAddSimAction::TankAddSimActionCount > 0) break;

		// Otherwise process the message
		AuthMessage *message = authMessages_.back();
		processMessageInternal(message->destinationId, message->ipAddress, message->message);
		authMessages_.pop_back();
		delete message;
	}

	while (!aiAdditions_.empty())
	{
		// Check that there are no outstanding tank addition requests
		if (TankAddSimAction::TankAddSimActionCount > 0) break;

		processAIInternal(aiAdditions_);
		aiAdditions_.clear();
	}
}

void ServerConnectAuthHandler::processMessageInternal(
	unsigned int destinationId,
	unsigned int ipAddress,
	ComsConnectAuthMessage &message)
{
	// Check for acceptance bassed on standard checks
	if (!ServerConnectHandler::checkStandardParams(destinationId, ipAddress)) return;

	// Check that this message has come from a destination that is still connected
	// This may happen if the client disconnects very early on in the initial setup
	ServerDestination *serverDestination = 
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (!serverDestination) 
	{
		return;
	}

	// Check player availability
	if (message.getNoPlayers() > 
		(unsigned int)
		(ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers() -
		ScorchedServer::instance()->getTargetContainer().getNoOfTanks()))
	{
		std::string kickMessage = 
			S3D::formatStringBuffer(
			"--------------------------------------------------\n"
			"This server is full, you cannot join!\n"
			"--------------------------------------------------");
		ServerCommon::serverLog("Server full, kicking");
		ServerCommon::kickDestination(destinationId, kickMessage);
		return;
	}

	// Auth handler, make sure that only prefered players can connect
	// DO FIRST AS THE HANDLER MAY ALTER THE MESSAGE
	ServerAuthHandler *authHandler =
		ScorchedServer::instance()->getAuthHandler();
	if (authHandler)
	{
		std::string resultMessage;
		if (!authHandler->authenticateUser(message, resultMessage))
		{
			std::string kickMessage = 
				S3D::formatStringBuffer(
				"--------------------------------------------------\n"
				"%s"
				"Connection failed.\n"
				"--------------------------------------------------",
				resultMessage.c_str());
			ServerCommon::kickDestination(destinationId, kickMessage);			
			return;
		}
	}

	std::string uniqueId = message.getUniqueId();
	if (!uniqueId.c_str()[0]) // No ID
	{
		// Generate the players unique id (if we need to)
		//
		// This will only actualy generate an id when using a persistent
		// stats logger as this is the only time we can be sure of not
		// giving out duplicate ids.
		//
		if (ScorchedServer::instance()->getEventHandlerDataBase())
		{
			uniqueId = ScorchedServer::instance()->getEventHandlerDataBase()->allocateId();
		}
	}
	std::string SUid = message.getSUI(); //request for SUI

	// Get compat ver
	unsigned int compatVer = message.getCompatabilityVer();
	// TODO send back proper version message

	// Check if this unique id has been banned
	// Do after auth handler as this may update uniqueid
	ServerBanned::BannedType type = 
		ScorchedServer::instance()->getBannedPlayers().getBanned(uniqueId.c_str(), SUid.c_str());
	if (type == ServerBanned::Banned)
	{
		ServerCommon::kickDestination(destinationId, 
			"Banned uniqueid/suid connection from destination");
		Logger::log(S3D::formatStringBuffer("UniqueId: %s, Suid: %s", 
			uniqueId.c_str(), SUid.c_str()));
		return;
	}

	// Check that this unique id has not already connected (if unique ids are in use)
	if (!ScorchedServer::instance()->getOptionsGame().getAllowSameUniqueId())
	{
		std::map<unsigned int, Tank *> &playingTanks = 
			ScorchedServer::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator playingItor;
		for (playingItor = playingTanks.begin();
			playingItor != playingTanks.end();
			++playingItor)
		{
			Tank *current = (*playingItor).second;
			if (current->getDestinationId() != 0)
			{
				if (uniqueId.c_str()[0])
				{
					if (0 == strcmp(current->getUniqueId(), uniqueId.c_str()))
					{
						ServerCommon::kickDestination(destinationId,
							"Duplicate uniqueid connection from destination");
						Logger::log(S3D::formatStringBuffer("UniqueId: %s, Suid: %s", 
							uniqueId.c_str(), SUid.c_str()));
						return;
					}
				}
				if (SUid.c_str()[0])
				{
					if (0 == strcmp(current->getSUI(), SUid.c_str()))
					{
						ServerCommon::kickDestination(destinationId,
							"Duplicate SUI connection from destination");
						Logger::log(S3D::formatStringBuffer("UniqueId: %s, Suid: %s", 
							uniqueId.c_str(), SUid.c_str()));
						return;
					}
				}
			}
		}
	}

	// Send the connection accepted message to the client
	ComsConnectAcceptMessage acceptMessage(
		destinationId,
		ScorchedServer::instance()->getOptionsGame().getServerName(),
		ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
		uniqueId.c_str());
	if (!ComsMessageSender::sendToSingleClient(acceptMessage, destinationId))
	{
		ServerCommon::kickDestination(destinationId,
			"Failed to send accept to client");
		return;
	}

	const char *savedGame = "";
#ifndef S3D_SERVER
	savedGame = ClientParams::instance()->getSaveFile();
#endif

	if (savedGame[0])
	{
		SaveGame::loadTargets(savedGame);
		ScorchedServer::instance()->getServerState().setState(ServerState::ServerNewLevelState);
	}
	else
	{
		// Add all the new tanks
		for (unsigned int i=0; i<message.getNoPlayers(); i++)
		{
			addNextTank(destinationId,
				ipAddress,	
				uniqueId.c_str(),
				SUid.c_str(),
				message.getHostDesc(),
				false);
		}

		// For the single player game
		// Add a spectator that will always remain a spectator
		// this is so if we only have computer players we still
		// send messages to them
	#ifndef S3D_SERVER
		{
			addNextTank(destinationId,
				ipAddress,
				uniqueId.c_str(),
				SUid.c_str(),
				message.getHostDesc(),
				true);
		}
	#endif
	}
}

void ServerConnectAuthHandler::addNextTank(unsigned int destinationId,
	unsigned int ipAddress,
	const char *sentUniqueId,
	const char *sentSUI,
	const char *sentHostDesc,
	bool extraSpectator)
{
	// The player has connected
	unsigned int tankId = 0;
	LangString playerName;
	if (extraSpectator)
	{
		tankId = TargetID::SPEC_TANK_ID;
		playerName = LANG_STRING("Spectator");
	}
	else
	{
		playerName = LANG_STRING(ScorchedServer::instance()->getTankAIStrings().getPlayerName());
		std::set<unsigned int> takenPlayerIds;
		tankId = TankAIAdder::getNextTankId(
			sentUniqueId,
			ScorchedServer::instance()->getContext(),
			takenPlayerIds);
	}

	// Make sure host desc does not contain \"
	for (char *h=(char *)sentHostDesc; *h; h++) if (*h == '\"') *h=' ';

	// Use the stats name if stats are enabled and the player has one
	std::list<std::string> aliases;
	if (ScorchedServer::instance()->getEventHandlerDataBase())
	{
		aliases = ScorchedServer::instance()->getEventHandlerDataBase()->
			getAliases(sentUniqueId);
	}
	if (!aliases.empty())
	{
		LangString alias = LANG_STRING(aliases.front());
		playerName = alias;
	}

	// Add this tank
	TankAddSimAction *simAction = new TankAddSimAction(tankId, destinationId,
		sentUniqueId, sentSUI, sentHostDesc, ipAddress, playerName, "");
	if (sentUniqueId[0])
	{
		ScorchedServer::instance()->getTankDeadContainer().getDeadTank(
			simAction, sentUniqueId);
	}
	else if (sentSUI[0])
	{
		ScorchedServer::instance()->getTankDeadContainer().getDeadTank(
			simAction, sentSUI);
	}

	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}

void ServerConnectAuthHandler::processAIInternal(std::list<std::string> &aiAdditions)
{
	std::set<std::string> takenUniqueIds;
	std::set<unsigned int> takenPlayerIds;
	std::list<std::string>::iterator itor;
	for (itor = aiAdditions.begin(); itor != aiAdditions.end(); ++itor)
	{
		std::string &aiName = *itor;
		TankAI *ai = ScorchedServer::instance()->getTankAIs().getAIByName(aiName.c_str());
		if (!ai)
		{
			Logger::log(S3D::formatStringBuffer("Failed to find a tank ai called \"%s\"",
				aiName.c_str()));
			return;
		}

		// Tank Name
		LangString newname = 
			LANG_STRING(ScorchedServer::instance()->getOptionsGame().getBotNamePrefix());
		newname += LANG_STRING(ScorchedServer::instance()->getTankAIStrings().getAIPlayerName(
			ScorchedServer::instance()->getContext()));

		// Unique Id
		char uniqueId[256];
		for (int i=1;;i++)
		{
			snprintf(uniqueId, 256, "%s - computer - %i", aiName.c_str(), i);
			if (takenUniqueIds.find(uniqueId) == takenUniqueIds.end() && !uniqueIdTaken(uniqueId)) break;
		}
		takenUniqueIds.insert(uniqueId);
		unsigned int playerId = TankAIAdder::getNextTankId(
			uniqueId, ScorchedServer::instance()->getContext(), takenPlayerIds);
		takenPlayerIds.insert(playerId);

		// Add this new tank
		TankAddSimAction *simAction = new TankAddSimAction(playerId, 0,
			uniqueId, "", "AI", 0, newname, aiName);
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
	}
}

bool ServerConnectAuthHandler::uniqueIdTaken(const std::string &uniqueId)
{
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator playingItor;
	for (playingItor = playingTanks.begin();
		playingItor != playingTanks.end();
		++playingItor)
	{
		Tank *current = (*playingItor).second;
		if (0 == strcmp(current->getUniqueId(), uniqueId.c_str()))
		{
			return true;
		}
	}
	return false;
}