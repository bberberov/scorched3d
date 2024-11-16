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

#include <net/NetServerTCPRead.h>
#include <net/NetServerTCP.h>
#include <net/NetMessagePool.h>
#include <common/Clock.h>
#include <common/Logger.h>
#include <common/Defines.h>

NetServerTCPRead::NetServerTCPRead(unsigned int id,
							 TCPsocket socket,
							 NetServerTCPProtocol *protocol,
							 NetMessageHandler *messageHandler,
							 bool *checkDeleted) : 
	id_(id),
	socket_(socket), sockSet_(0), protocol_(protocol), 
	outgoingMessagesMutex_(0), checkDeleted_(checkDeleted),
	disconnect_(false), messageHandler_(messageHandler),
	sentDisconnect_(false), startCount_(0),
	ctrlThread_(0), recvThread_(0), sendThread_(0)
{
	sockSet_ = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(sockSet_, socket);
	outgoingMessagesMutex_ = SDL_CreateMutex();
}

NetServerTCPRead::~NetServerTCPRead()
{
	SDL_LockMutex(outgoingMessagesMutex_);
	while (!newMessages_.empty())
	{
		NetMessage *message = newMessages_.front();
		newMessages_.pop_front();
		NetMessagePool::instance()->addToPool(message);
	}
	SDL_UnlockMutex(outgoingMessagesMutex_);

	SDL_DestroyMutex(outgoingMessagesMutex_);
	outgoingMessagesMutex_ = 0;
	SDLNet_FreeSocketSet(sockSet_);
	sockSet_ = 0;
}

unsigned int NetServerTCPRead::getIpAddressFromSocket(TCPsocket socket)
{
	unsigned int addr = 0;
	IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
	if (address)
	{
		addr = SDLNet_Read32(&address->host);
	}
	return addr;
}

unsigned int NetServerTCPRead::getIpAddress()
{
	return getIpAddressFromSocket(socket_);
}

void NetServerTCPRead::start()
{
	// Send the player connected notification
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::ConnectMessage, 
		id_,
		getIpAddress());
	messageHandler_->addMessage(message);

	recvThread_ = SDL_CreateThread(
		NetServerTCPRead::recvThreadFunc, (void *) this);
	sendThread_ = SDL_CreateThread(
		NetServerTCPRead::sendThreadFunc, (void *) this);
	ctrlThread_ = SDL_CreateThread(
		NetServerTCPRead::ctrlThreadFunc, (void *) this);
	if (!ctrlThread_ || !recvThread_ || !sendThread_)
	{
		Logger::log( "ERROR: Run out of threads");
	}
}

void NetServerTCPRead::addMessage(NetMessage *message)
{
	if (message->getMessageType() != NetMessage::DisconnectMessage && 
		message->getBuffer().getBuffer() == 0) 
	{ 
		DIALOG_ASSERT(0); 
	}

	SDL_LockMutex(outgoingMessagesMutex_);
	newMessages_.push_back(message);
	if (message->getMessageType() == NetMessage::DisconnectMessage &&
		!sentDisconnect_)
	{
		sentDisconnect_ = true;
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				id_,
				getIpAddress());
		messageHandler_->addMessage(message);
	}
	SDL_UnlockMutex(outgoingMessagesMutex_);
}

bool NetServerTCPRead::getDisconnect()
{ 
	SDL_LockMutex(outgoingMessagesMutex_);
	bool result = disconnect_;	
	SDL_UnlockMutex(outgoingMessagesMutex_);

	if (result)
	{
		int status = 0;
		SDL_WaitThread(ctrlThread_, &status);
	}
	return result; 
}

int NetServerTCPRead::ctrlThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualCtrlThreadFunc();
	return 0;
}

int NetServerTCPRead::sendThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualSendRecvThreadFunc(true);
	return 0;
}

int NetServerTCPRead::recvThreadFunc(void *netServerTCPRead)
{
	NetServerTCPRead *ns = (NetServerTCPRead *) netServerTCPRead;
	ns->actualSendRecvThreadFunc(false);
	return 0;
}

void NetServerTCPRead::actualCtrlThreadFunc()
{
	// Ensure the other threads have started
	bool done = false;
	while (!done)
	{
		SDL_LockMutex(outgoingMessagesMutex_);
		if (startCount_ == 2) done = true;
		SDL_UnlockMutex(outgoingMessagesMutex_);
		SDL_Delay(100);
	}

	// Wait for the other threads to end
	int status;
	SDL_WaitThread(sendThread_, &status);
	SDL_WaitThread(recvThread_, &status);

	// Tidy socket
	SDLNet_TCP_Close(socket_);

	// Delete self
	SDL_LockMutex(outgoingMessagesMutex_);
	disconnect_ = true;
	*checkDeleted_ = true;
	SDL_UnlockMutex(outgoingMessagesMutex_);
}

void NetServerTCPRead::actualSendRecvThreadFunc(bool send)
{
	SDL_LockMutex(outgoingMessagesMutex_);
	startCount_++;
	SDL_UnlockMutex(outgoingMessagesMutex_);

	Clock netClock;
	while (!sentDisconnect_)
	{
		netClock.getTimeDifference();

		if (send)
		{	
			if (!pollOutgoing()) break;
		}
		else
		{
			if (!pollIncoming()) break;
		}

		float timeDiff = netClock.getTimeDifference();
		if (timeDiff > 15.0f)
		{
			Logger::log(S3D::formatStringBuffer(
				"Warning: %s net loop took %.2f seconds, client %u", 
				(send?"Send":"Recv"),
				timeDiff, id_));
		}
	}

	SDL_LockMutex(outgoingMessagesMutex_);
	if (!sentDisconnect_)
	{
		sentDisconnect_ = true;
		NetMessage *message = NetMessagePool::instance()->
			getFromPool(NetMessage::DisconnectMessage, 
				id_,
				getIpAddress());
		messageHandler_->addMessage(message);
	}
	SDL_UnlockMutex(outgoingMessagesMutex_);
}

bool NetServerTCPRead::pollIncoming()
{
	int numready = SDLNet_CheckSockets(sockSet_, 100);
	if (numready == -1) return false;
	if (numready == 0)
	{
		SDL_Delay(10);
		return true;
	}

	if(SDLNet_SocketReady(socket_))
	{
		NetMessage *message = protocol_->readBuffer(socket_, id_);
		if (!message)
		{
			//Logger::log( "Client socket has been closed.");
			return false;
		}
		else
		{
			if (!sentDisconnect_)
			{
				// We have a buffer containing the message
				messageHandler_->addMessage(message);
			}
			else
			{
				NetMessagePool::instance()->addToPool(message);
			}
		}
	}

	return true;
}

bool NetServerTCPRead::pollOutgoing()
{
	NetMessage *message = 0;
	SDL_LockMutex(outgoingMessagesMutex_);
	if (!newMessages_.empty())
	{
		message = newMessages_.front();
		newMessages_.pop_front();
	}
	SDL_UnlockMutex(outgoingMessagesMutex_);

	bool result = true;
	if (message)
	{
		if (message->getMessageType() == NetMessage::DisconnectMessage)
		{
			result = false;
		}
		else
		{
			if (!protocol_->sendBuffer(message->getBuffer(), socket_, id_))
			{
				Logger::log( "Failed to send message to client");
				result = false;
			}
		}
		message->setType(NetMessage::SentMessage);
		messageHandler_->addMessage(message);
	}
	else SDL_Delay(100);

	return result;
}
