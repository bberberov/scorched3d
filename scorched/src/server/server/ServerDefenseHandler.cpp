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

#include <server/ServerDefenseHandler.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <simactions/TankDefenseSimAction.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <common/Logger.h>

ServerDefenseHandler::ServerDefenseHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsDefenseMessage::ComsDefenseMessageType,
		this);
}

ServerDefenseHandler::~ServerDefenseHandler()
{
}

bool ServerDefenseHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Decode the connect message
	ComsDefenseMessage message;
	if (!message.readMessage(reader)) return false;

	// Get the correct player id from the message
	unsigned int playerId = message.getPlayerId();

	// Check we are in the correct state
	if ((ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerPlayingState) &&
		(ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerBuyingState) &&
		(ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerTankNewGameState))
	{
		Logger::log("ERROR: Player attempted to use defense but in incorrect state");
		return true;
	}

	// Check tank exists 
	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	if (!tank || tank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log("ERROR: Player using defense does not exist at this destination");
		return true;
	}

	// Fire defense
	TankDefenseSimAction *simAction = new TankDefenseSimAction(message);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);

	return true;
}
