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

#include <net/NetServerTCP3Destination.h>
#include <common/Logger.h>

NetServerTCP3Destination::NetServerTCP3Destination(
	NetMessageHandler *recieveMessageHandler, 
	TCPsocket socket,
	unsigned int destinationId) :
	socket_(socket), destinationId_(destinationId),
	send_(socket, destinationId, 
		getIpAddressFromSocket(socket), recieveMessageHandler),
	recv_(socket, destinationId, 
		getIpAddressFromSocket(socket), recieveMessageHandler),
	running_(true)
{
	ipAddress_ = getIpAddressFromSocket(socket_);
}

NetServerTCP3Destination::~NetServerTCP3Destination()
{
}

unsigned int NetServerTCP3Destination::getIpAddressFromSocket(TCPsocket socket)
{
	unsigned int addr = 0;
	IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
	if (address)
	{
		addr = SDLNet_Read32(&address->host);
	}
	return addr;
}

void NetServerTCP3Destination::printStats()
{
	Logger::log(S3D::formatStringBuffer("TCP3 Destination %u net stats:", 
		destinationId_));
	Logger::log(S3D::formatStringBuffer("  %u messages sent, %u bytes out",
		send_.getMessagesSent(), send_.getBytesOut()));
	Logger::log(S3D::formatStringBuffer("  %u messages recieved, %u bytes in",
		recv_.getMessagesRecieved(), recv_.getBytesIn()));
}

void NetServerTCP3Destination::sendMessage(NetMessage *message) 
{
	send_.sendMessage(message);
}

void NetServerTCP3Destination::close(NetMessage *message)
{
	DIALOG_ASSERT(message->getMessageType() == NetMessage::DisconnectMessage);
	send_.sendMessage(message);
	recv_.stop();
	running_ = false;
}

bool NetServerTCP3Destination::anyFinished()
{
	if (!running_ || send_.getStopped() || recv_.getStopped())
	{
		running_ = false;
		return true;
	}
	return false;
}

bool NetServerTCP3Destination::allFinished()
{
	if (send_.getStopped() && recv_.getStopped())
	{
		send_.wait();
		recv_.wait();
		SDLNet_TCP_Close(socket_);
		return true;
	}
	return false;
}
