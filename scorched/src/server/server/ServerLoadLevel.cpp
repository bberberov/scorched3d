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

#include <server/ServerLoadLevel.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerDestinations.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/Simulator.h>
#include <simactions/TankLoadedSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsLoadLevelMessage.h>
#include <coms/ComsLevelLoadedMessage.h>

ServerLoadLevel::ServerLoadLevel(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsLevelLoadedMessage::ComsLevelLoadedMessageType,
		this);
}

ServerLoadLevel::~ServerLoadLevel()
{
}

void ServerLoadLevel::destinationLoadLevel(unsigned int destinationId)
{
	if (destinationId == 0)
	{
		setLoaded(destinationId);
	}
	else
	{
		LandscapeDefinition &landscapeDefinition =
			ScorchedServer::instance()->getLandscapeMaps().getDefinitions().
			getDefinition();
		if (landscapeDefinition.getDefinitionNumber() != 0)
		{
			// Set any tanks from this destination that they are loading the level
			// set the current version of the level (definition number) that they are loading
			// so we can check if the level changes before we have finished loading
			ServerDestination *destination =
				ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
			if (destination)
			{
				destination->setState(ServerDestination::sLoadingLevel);
				destination->setLevelNumber(landscapeDefinition.getDefinitionNumber());
			}

			// Tell this destination to start loading the level
			ComsLoadLevelMessage &loadLevelMessage = 
				ScorchedServer::instance()->getServerSimulator().getLevelMessage();
			ComsMessageSender::sendToSingleClient(loadLevelMessage, destinationId, NetInterfaceFlags::fCompress);
		}
	}
}

bool ServerLoadLevel::destinationUsingCurrentLevel(unsigned int destinationId)
{
	LandscapeDefinition &landscapeDefinition =
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions().
		getDefinition();

	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (destination)
	{
		return (destination->getLevelNumber() == landscapeDefinition.getDefinitionNumber());
	}
	return false;
}

bool ServerLoadLevel::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	unsigned int destinationId = netMessage.getDestinationId();

	// Check that the destination is still using the correct level
	// the server may have changed level during the time taken for the 
	// client to load the level
	if (!destinationUsingCurrentLevel(destinationId)) 
	{
		destinationLoadLevel(destinationId);
		return true;
	}

	// This destination has finished loading
	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().getDestination(destinationId);
	if (destination)
	{
		destination->setState(ServerDestination::sFinished);
	}

	setLoaded(destinationId);

	return true;
}

void ServerLoadLevel::setLoaded(unsigned int destinationId)
{
	// These tanks are now ready to play
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		// For each tank
		Tank *tank = (*itor).second;
		if (destinationId == tank->getDestinationId())
		{
			TankLoadedSimAction *loadedAction = 
				new TankLoadedSimAction(tank->getPlayerId(), 
					tank->getState().getNotSpectator());
			ScorchedServer::instance()->getServerSimulator().
				addSimulatorAction(loadedAction);
		}
	}
}
