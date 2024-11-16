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

#include <server/ServerStateEnoughPlayers.h>
#include <server/ServerConnectAuthHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerCommon.h>
#include <server/ServerMessageHandler.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAI.h>
#include <tankai/TankAIAdder.h>
#include <simactions/TankAddSimAction.h>
#include <simactions/TankRemoveSimAction.h>
#include <simactions/TankChangeSimAction.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

ServerStateEnoughPlayers::ServerStateEnoughPlayers()
{
}

ServerStateEnoughPlayers::~ServerStateEnoughPlayers()
{
}

ServerStateEnoughPlayers::Result ServerStateEnoughPlayers::enoughPlayers()
{
	if (TankAddSimAction::TankAddSimActionCount == 0 &&
		TankRemoveSimAction::TankRemoveSimActionCount == 0 &&
		TankChangeSimAction::TankChangeSimActionCount == 0 &&
		!ScorchedServer::instance()->getServerConnectAuthHandler().outstandingRequests())
	{
		// Check if we need to add or remove bots to keep game going
		// Any bots added won't join until the next round anyway
		ballanceBots(ScorchedServer::instance()->getContext());
	}

	// Only perform checks if nothing no players are being added or removed
	if (TankAddSimAction::TankAddSimActionCount != 0 ||
		TankRemoveSimAction::TankRemoveSimActionCount != 0 ||
		TankChangeSimAction::TankChangeSimActionCount != 0 ||
		ScorchedServer::instance()->getServerConnectAuthHandler().outstandingRequests())
	{
		return ServerStateEnoughPlayers::eWaiting;
	}

	// Make sure we have enough players to play a game
	if (ScorchedServer::instance()->getTargetContainer().getNoOfNonSpectatorTanks() <
		ScorchedServer::instance()->getOptionsGame().getNoMinPlayers())
	{
		checkExit();
		return ServerStateEnoughPlayers::eNotEnough;
	}
	
	// Check we have enough team players
	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAuto &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAutoByScore &&
		ScorchedServer::instance()->getOptionsGame().getTeamBallance() !=
			OptionsGame::TeamBallanceAutoByBots)
	{
		// If it is auto ballanced, then if there are at least two players
		// then we are ok.  And if there are not two players then
		// the first check will catch it.
				
		// Check there is at least one player in each team
		int teamCount[4];
		for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams();i++)
		{
			teamCount[i] = 0;
		}
		std::map<unsigned int, Tank *> &playingTanks = 
			ScorchedServer::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator mainitor;
		for (mainitor = playingTanks.begin();
			 mainitor != playingTanks.end();
			 ++mainitor)
		{
			Tank *current = (*mainitor).second;
			if (current->getState().getTankPlaying() &&
				current->getTeam() > 0)
			{
				teamCount[current->getTeam() - 1]++;
			}
		}
		for (int i=0; i<ScorchedServer::instance()->getOptionsGame().getTeams();i++)
		{
			if (teamCount[i] == 0)
			{
				checkExit();
				return ServerStateEnoughPlayers::eNotEnough;
			}
		}
	}
	
	return ServerStateEnoughPlayers::eEnough;
}

void ServerStateEnoughPlayers::ballanceBots(ScorchedContext &context)
{
	// Get the number of players we require
	int requiredPlayers =
		context.getOptionsGame().getRemoveBotsAtPlayers();
	if (requiredPlayers == 0) return;

	// Get the number of players we have
	int noPlayers = countBots(context);

	// Check if we need to gain or lose players
	if (noPlayers > requiredPlayers)
	{
		removeBots(requiredPlayers, noPlayers);
	}
	else if (noPlayers < requiredPlayers)
	{
		addBots(requiredPlayers, noPlayers);
	}
}

int ServerStateEnoughPlayers::countBots(ScorchedContext &context)
{
	// Add up the number of tanks that are either
	// human and not spectators
	// or an ai
	int noPlayers = 0;
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if ((current->getDestinationId() != 0 && current->getState().getTankPlaying()) ||
			(current->getDestinationId() == 0 && !current->getTankAI()->removedPlayer()))
		{
			noPlayers++;
		}
	}
	return noPlayers;
}

void ServerStateEnoughPlayers::removeBots(int requiredPlayers, int noPlayers)
{
	std::multimap<unsigned int, unsigned int> ais_;

	// Get this list of computer players and sort them
	// by the time they have been playing for
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getDestinationId() == 0 &&
			!current->getTankAI()->removedPlayer())
		{
			unsigned int startTime = (unsigned int)
				current->getScore().getStartTime();
			ais_.insert(std::pair<const unsigned int, unsigned int>
				(startTime, current->getPlayerId()));
		}
	}

	// Kick the ais that have been on the server the longest
	std::multimap<unsigned int, unsigned int>::reverse_iterator aiItor;
	for (aiItor = ais_.rbegin(); 
		noPlayers > requiredPlayers && aiItor != ais_.rend(); 
		++aiItor, noPlayers--)
	{
		std::pair<const unsigned int, unsigned int> item = *aiItor;
		ScorchedServer::instance()->getServerMessageHandler().destroyPlayer(
			item.second, "Auto-kick");
	}
}

void ServerStateEnoughPlayers::addBots(int requiredPlayers, int noPlayers)
{
	std::multimap<std::string, unsigned int> ais_;

	// Get this list of computer players and sort them
	// by ai name
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		mainitor != playingTanks.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getDestinationId() == 0 &&
			!current->getTankAI()->removedPlayer())
		{
			ais_.insert(std::pair<const std::string, unsigned int>
				(current->getTankAI()->getName(), 
				current->getPlayerId()));
		}
	}

	// Add any computer players that should be playing 
	// and that are not in the list of currently playing
	int maxComputerAIs = 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers();
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human") &&
			0 != stricmp(playerType, "Random"))
		{
			std::multimap<std::string, unsigned int>::iterator findItor =
				ais_.find(playerType);
			if (findItor == ais_.end())
			{
				if (noPlayers < requiredPlayers)
				{
					// This player does not exist add them
					TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
					noPlayers++;
				}
			}
			else
			{
				// This player does exist dont add them
				ais_.erase(findItor);
			}
		}
	}
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human") &&
			0 == stricmp(playerType, "Random"))
		{
			if (ais_.empty())
			{
				if (noPlayers < requiredPlayers)
				{
					// This player does not exist add them
					TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
					noPlayers++;
				}										
			}
			else
			{
				// This player does exist dont add them
				ais_.erase(ais_.begin());
			}
		}
	}
}

void ServerStateEnoughPlayers::checkExit()
{
	if (ServerCommon::getExitEmpty())
	{
		Logger::log("Exit server when empty");
		exit(0);
	}
}
