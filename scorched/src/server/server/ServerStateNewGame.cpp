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

#include <server/ServerStateNewGame.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <server/ServerCommon.h>
#include <server/ServerSimulator.h>
#include <server/ServerConsoleProgressCounter.h>
#include <server/ServerChannelManager.h>
#include <server/ServerLoadLevel.h>
#include <weapons/EconomyStore.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/OptionsTransient.h>
#include <events/EventController.h>

ServerStateNewGame::ServerStateNewGame()
{
}

ServerStateNewGame::~ServerStateNewGame()
{
}

void ServerStateNewGame::newGame()
{
	newGameState();

	// Make sure any remaining actions have been processed before starting a new game
	ScorchedServer::instance()->getServerSimulator().processRemaining();

	// Make sure tanks are in correct state
	std::list<Tank *> playingTanks;
	std::set<unsigned int> loadingDestinations;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		++tankItor)
	{
		Tank *tank = tankItor->second;
		if (tank->getState().getState() != TankState::sLoading)
		{
			tank->getState().setState(TankState::sLoading);
			if (loadingDestinations.find(tank->getDestinationId()) == loadingDestinations.end())
			{
				loadingDestinations.insert(tank->getDestinationId());
				playingTanks.push_back(tank);
			}
		}
	}

	// Inform the stats logger
	ScorchedServer::instance()->getEventController().gameStart(playingTanks);

	// Store this as the current level
	// Do after setting the state so the state of the tanks is consistent when 
	// the level is saved
	ScorchedServer::instance()->getServerSimulator().newLevel();

	// Tell all destinations to load
	std::set<unsigned int>::iterator destItor;
	for (destItor = loadingDestinations.begin();
		destItor != loadingDestinations.end();
		++destItor)
	{
		ServerLoadLevel::destinationLoadLevel(*destItor);
	}
}

void ServerStateNewGame::newGameState()
{
	// Make sure options are up to date
	if (ScorchedServer::instance()->getOptionsGame().commitChanges())
	{
		ScorchedServer::instance()->getServerChannelManager().sendText(
			ChannelText("info", 
				"GAME_OPTIONS_CHANGED", 
				"Game options have been changed!"),
			true);
	}	

	// Make any enconomic changes
	ScorchedServer::instance()->getEconomyStore().getEconomy()->calculatePrices();
	ScorchedServer::instance()->getEconomyStore().getEconomy()->savePrices();

	// Get a landscape definition to use
	ServerCommon::serverLog("Generating landscape");
	LandscapeDefinition defn;
	if (ScorchedServer::instance()->getLandscapeMaps().getDefinitions().
		getDefinition().getDefinitionNumber() == 0)
	{
		defn = ScorchedServer::instance()->getLandscapes().getBlankLandscapeDefn();
	}
	else
	{
		defn = ScorchedServer::instance()->getLandscapes().getRandomLandscapeDefn(
			ScorchedServer::instance()->getContext().getOptionsGame(),
			ScorchedServer::instance()->getContext().getTargetContainer());
	}

	// Load the per level options
	ScorchedServer::instance()->getOptionsGame().updateLevelOptions(
		ScorchedServer::instance()->getContext(), defn);

	// Set all options (wind etc..)
	if (ScorchedServer::instance()->getServerState().getState() == ServerState::ServerStartupState)
	{
		ScorchedServer::instance()->getContext().getOptionsTransient().reset();
	}
	ScorchedServer::instance()->getContext().getOptionsTransient().newGame();

	// Generate the new level
	ProgressCounter *progressCounter = 0;
#ifdef S3D_SERVER
	progressCounter = ServerConsoleProgressCounter::instance()->getProgressCounter();
#endif

	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), defn,
		progressCounter);
	ServerCommon::serverLog("Finished generating landscape");
}
