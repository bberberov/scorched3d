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

#ifndef S3D_SERVER
	#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <weapons/AccessoryStore.h>
#include <common/OptionsScorched.h>

ComsMessageType ComsConnectAcceptMessage::ComsConnectAcceptMessageType("ComsConnectAcceptMessageType");

ComsConnectAcceptMessage::ComsConnectAcceptMessage(unsigned int destinationId,
	const char *serverName,
	const char *publishAddress,
	const char *uniqueId) :
	ComsMessage(ComsConnectAcceptMessageType),
	destinationId_(destinationId),
	serverName_(serverName),
	publishAddress_(publishAddress),
	uniqueId_(uniqueId)
{

}

ComsConnectAcceptMessage::~ComsConnectAcceptMessage()
{

}

bool ComsConnectAcceptMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(serverName_);
	buffer.addToBuffer(publishAddress_);
	buffer.addToBuffer(uniqueId_);
	if (!ScorchedServer::instance()->getOptionsGame().getMainOptions().
		writeToBuffer(buffer, false, false)) return false;
	return true;
}

bool ComsConnectAcceptMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(destinationId_)) return false;
	if (!reader.getFromBuffer(serverName_)) return false;
	if (!reader.getFromBuffer(publishAddress_)) return false;
	if (!reader.getFromBuffer(uniqueId_)) return false;
#ifndef S3D_SERVER
	if (!ScorchedClient::instance()->getOptionsGame().getMainOptions().
		readFromBuffer(reader, false, false)) return false;
#endif // #ifndef S3D_SERVER
	return true;
}
