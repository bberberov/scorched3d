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

#include <server/ServerBuyAccessoryHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <server/ServerSimulator.h>
#include <simactions/TankAccessorySimAction.h>
#include <common/Logger.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>

ServerBuyAccessoryHandler::ServerBuyAccessoryHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsBuyAccessoryMessage::ComsBuyAccessoryMessageType,
		this);
}

ServerBuyAccessoryHandler::~ServerBuyAccessoryHandler()
{
}

bool ServerBuyAccessoryHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsBuyAccessoryMessage message;
	if (!message.readMessage(reader)) return false;
	unsigned int playerId = message.getPlayerId();

	// Check we are at the correct time to buy anything
	if ((ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerBuyingState) &&
		(ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerTankNewGameState))
	{
		Logger::log( "ERROR: Player attempted to buy accessory but in incorrect state");
		return true;
	}

	// Check that is player still exists
	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	if (!tank)
	{
		Logger::log( "ERROR: Player buying does not exist");
		return true;
	}

	// Check that the message comes from the right destination
	if (tank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log( "ERROR: Player buying does not exist at this destination");
		return true;
	}

	if (tank->getState().getState() != TankState::sBuying) 
	{
		Logger::log( "ERROR: Player buying when not in buying state");
		return true;
	}

	// Add/rm accessory to the tanks
	TankAccessorySimAction *simAction = new TankAccessorySimAction(message);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);

	return true;
}
