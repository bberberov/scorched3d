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

#include <server/ServerGiftMoneyHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <simactions/TankGiftSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Logger.h>

ServerGiftMoneyHandler::ServerGiftMoneyHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsGiftMoneyMessage::ComsGiftMoneyMessageType,
		this);
}

ServerGiftMoneyHandler::~ServerGiftMoneyHandler()
{
}

bool ServerGiftMoneyHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsGiftMoneyMessage message;
	if (!message.readMessage(reader)) return false;

	unsigned int fromPlayerId = message.getFromPlayerId();

	// Check we are at the correct time to buy anything
	if ((ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerBuyingState) &&
		(ScorchedServer::instance()->getServerState().getState() != 
		ServerState::ServerTankNewGameState))
	{
		Logger::log( "ERROR: Player attempted to gift money but in incorrect state");
		return true;
	}

	// Check that is player still exists
	Tank *fromTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(fromPlayerId);
	if (!fromTank)
	{
		Logger::log( "ERROR: Player gifting does not exist");
		return true;
	}
	if (fromTank->getDestinationId() != netMessage.getDestinationId())
	{
		Logger::log( "ERROR: Player gifting does not exist at this destination");
		return true;
	}
	if (fromTank->getState().getState() != TankState::sBuying) 
	{
		Logger::log( "ERROR: Player gifting when not in buying state");
		return true;
	}

	// Send message
	TankGiftSimAction *action = new TankGiftSimAction(message);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}
