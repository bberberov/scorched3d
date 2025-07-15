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

#include <server/ServerLinesHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerDestinations.h>
#include <coms/ComsLinesMessage.h>
#include <coms/ComsMessageSender.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Logger.h>
#include <set>

ServerLinesHandler::ServerLinesHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsLinesMessage::ComsLinesMessageType,
		this);
}

ServerLinesHandler::~ServerLinesHandler()
{
}

bool ServerLinesHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsLinesMessage message;
	if (!message.readMessage(reader)) return false;

	// Reject large messages
	if (message.getLines().size() > 150) return true;

	// Check the player id has been supplied
	unsigned int playerId = message.getPlayerId();
	if (playerId == 0) return true;

	// Check this is a tank, and comes from the correct destination
	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	if (!tank || tank->getDestinationId() != netMessage.getDestinationId()) return true;

	// If this tank has been muted also don't allow lines
	if (tank->getState().getMuted()) return true;

	// Get destinationinfo for this tank
	ServerDestination *destinationInfo =
		ScorchedServer::instance()->getServerDestinations().getDestination(netMessage.getDestinationId());

	// Send all team messages to everyone in the team (or any admins)
	// Only send to the same destination once
	std::set<unsigned int> doneDests;
	doneDests.insert(netMessage.getDestinationId()); // Don't send to recieved dest
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *currentTank = (*itor).second;
		if (tank->getTeam() == currentTank->getTeam() ||
			(destinationInfo && destinationInfo->getAdmin()))
		{
			if (doneDests.find(currentTank->getDestinationId()) ==
				doneDests.end())
			{
				doneDests.insert(currentTank->getDestinationId());
				ComsMessageSender::sendToSingleClient(message,
					currentTank->getDestinationId());
			}
		}
	}	

	return true;
}
