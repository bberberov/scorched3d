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

#include <net/NetLoopBack.h>
#include <net/NetMessagePool.h>
#include <common/Defines.h>
#include <common/Logger.h>

static unsigned int ClientLoopBackID = 100001;
static unsigned int ServerLoopBackID = 200002;

NetLoopBack *NetLoopBack::serverLoopback_(0);
NetLoopBack *NetLoopBack::clientLoopback_(0);

NetLoopBack::NetLoopBack(bool server) 
	: server_(server), started_(true)
{
	if (server_) serverLoopback_ = this;
	else clientLoopback_ = this;
}

NetLoopBack::~NetLoopBack()
{
	if (server_) serverLoopback_ = 0;
	else clientLoopback_ = 0;
}

bool NetLoopBack::connect(const char *hostName, int portNo)
{
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::ConnectMessage, ServerLoopBackID, 0);
		messageHandler_.addMessage(message);
	}
	if (getLoopback())
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::ConnectMessage, ClientLoopBackID, 0);
		getLoopback()->messageHandler_.addMessage(message);
	}
	return true;
}

bool NetLoopBack::started()
{
	return started_;
}

void NetLoopBack::setMessageHandler(NetMessageHandlerI *handler) 
{ 
	messageHandler_.setMessageHandler(handler); 
}

int NetLoopBack::processMessages()
{ 
	return messageHandler_.processMessages(); 
}

void NetLoopBack::disconnectAllClients()
{
	if (getLoopback())
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				server_?ServerLoopBackID:ClientLoopBackID, 0, 
				(unsigned int) 0);
		getLoopback()->messageHandler_.addMessage(message);
	}
	{
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				server_?ClientLoopBackID:ServerLoopBackID, 0, 
				(unsigned int) 0);
		messageHandler_.addMessage(message);
	}
}

NetLoopBack *NetLoopBack::getLoopback()
{
	if (server_) return clientLoopback_;
	else return serverLoopback_;
}

void NetLoopBack::disconnectClient(unsigned int client)
{
	Logger::log(S3D::formatStringBuffer("Cannot disconnect client %i, they are local!", client));
}

void NetLoopBack::disconnectClient(NetBuffer &buffer, unsigned int client)
{
	disconnectClient(client);
}

void NetLoopBack::sendMessageServer(NetBuffer &buffer, 
	unsigned int flags)
{
	sendMessageDest(buffer, ServerLoopBackID, flags);
}

void NetLoopBack::sendMessageDest(NetBuffer &buffer, 
	unsigned int destination, unsigned int flags)
{
	DIALOG_ASSERT(
		getLoopback() &&
		(server_ && destination == ClientLoopBackID) || 
		(!server_ && destination == ServerLoopBackID));

	unsigned int recvTime = (unsigned int) SDL_GetTicks();
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, 
			server_?ServerLoopBackID:ClientLoopBackID, 
			0, flags, recvTime);
	message->getBuffer().reset();
	message->getBuffer().addDataToBuffer(buffer.getBuffer(), buffer.getBufferUsed());
	getLoopback()->messageHandler_.addMessage(message);
}

