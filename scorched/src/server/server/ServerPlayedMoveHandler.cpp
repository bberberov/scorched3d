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

#include <server/ServerPlayedMoveHandler.h>
#include <server/ServerStateBuying.h>
#include <server/ServerStatePlaying.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>

ServerPlayedMoveHandler::ServerPlayedMoveHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsPlayedMoveMessage::ComsPlayedMoveMessageType,
		this);
}

ServerPlayedMoveHandler::~ServerPlayedMoveHandler()
{
}

bool ServerPlayedMoveHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsPlayedMoveMessage message;
	if (!message.readMessage(reader)) return false;

	Tank *tank = ScorchedServer::instance()->getTargetContainer().
		getTankById(message.getPlayerId());
	if (!tank) return true;
	if (tank->getDestinationId() != netMessage.getDestinationId()) return true;

	if (message.getType() == ComsPlayedMoveMessage::eFinishedBuy)
	{
		ScorchedServer::instance()->getServerState().buyingFinished(message);
	}
	else
	{
		ScorchedServer::instance()->getServerState().moveFinished(message);
	}

	return true;
}
