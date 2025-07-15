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

#include <server/ServerConnectHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerAuthHandler.h>
#include <server/ServerBanned.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <target/TargetContainer.h>
#include <coms/ComsConnectAuthMessage.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>

ServerConnectHandler::ServerConnectHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsConnectMessage::ComsConnectMessageType,
		this);
}

ServerConnectHandler::~ServerConnectHandler()
{
}

bool ServerConnectHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	unsigned int destinationId = netMessage.getDestinationId();
	unsigned int ipAddress = netMessage.getIpAddress();

	// Check for acceptance bassed on standard checks
	if (!checkStandardParams(destinationId, ipAddress)) return true;

	// Decode the connect message
	ComsConnectMessage message;

	// Check the player protocol versions are the same (correct)
	if (!message.readMessage(reader) ||
		(0 != strcmp(message.getProtocolVersion(), S3D::ScorchedProtocolVersion.c_str())))
	{
		std::string kickMessage = 
			S3D::formatStringBuffer(
			"--------------------------------------------------\n"
			"The version of Scorched you are running\n"
			"does not match the server's version.\n"
			"This server is running Scorched build %s (%s).\n"
			"You are running Scorched build %s (%s).\n"
			"New versions can be downloaded from\n"
			"http://www.scorched3d.co.uk\n"
			"Connection failed.\n"
			"--------------------------------------------------", 
			S3D::ScorchedVersion.c_str(), S3D::ScorchedProtocolVersion.c_str(),
			message.getVersion(), message.getProtocolVersion());
		Logger::log( 
			S3D::formatStringBuffer(
			"Player connected with out of date version \"%s(%s)\"",
			message.getVersion(),
			message.getProtocolVersion()));

		ServerCommon::kickDestination(destinationId, kickMessage);
		return true;
	}

	// Tell the client to continue auth connection
	ComsConnectAuthMessage comsConnectAuthMessage;
	ServerAuthHandler *authHandler = ScorchedServer::instance()->getAuthHandler();
	if (authHandler)
	{
		authHandler->createAuthentication(comsConnectAuthMessage);
	}
	if (!ComsMessageSender::sendToSingleClient(
		comsConnectAuthMessage, destinationId)) return false;

	return true;
}

bool ServerConnectHandler::checkStandardParams(unsigned int destinationId, unsigned int ipAddress)
{
	// Only do this on the server, the client can have all bots
#ifdef S3D_SERVER
	{
		// First things, first
		// Check we can actually accept the connection
		if (ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers() <=
			ScorchedServer::instance()->getTargetContainer().getNoOfTanks())
		{
			const char *kickMessage =
				"--------------------------------------------------\n"
				"This server is full, you cannot join!\n"
				"--------------------------------------------------";
			ServerCommon::serverLog("Server full, kicking");
			ServerCommon::kickDestination(destinationId, kickMessage);
			return false;		
		}
	}
#endif // S3D_SERVER

	// Check if this ip address has been banned
	ServerBanned::BannedType type = 
		ScorchedServer::instance()->getBannedPlayers().getBanned(ipAddress);
	if (type == ServerBanned::Banned)
	{
		ServerCommon::kickDestination(destinationId,
			"Banned ipaddress connection from destination");
		return false;
	}

	return true;
}

