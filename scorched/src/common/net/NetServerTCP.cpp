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

#include <net/NetServerTCP.h>
#include <net/NetBufferUtil.h>
#include <net/NetMessageHandler.h>
#include <net/NetMessagePool.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/Clock.h>

NetServerTCP::NetServerTCP(NetServerTCPProtocol *protocol) : 
	sockSet_(0), firstDestination_(0),
	server_(0), protocol_(protocol), checkDeleted_(false),
	lastId_(0)
{
	sockSet_ = SDLNet_AllocSocketSet(1);
	setMutex_ = SDL_CreateMutex();
	SDL_CreateThread(NetServerTCP::threadFunc, (void *) this);
}

NetServerTCP::~NetServerTCP()
{
	SDL_DestroyMutex(setMutex_); 
	setMutex_ = 0;
	SDLNet_FreeSocketSet(sockSet_);
	sockSet_ = 0;
}

void NetServerTCP::setMessageHandler(NetMessageHandlerI *handler) 
{ 
	messageHandler_.setMessageHandler(handler); 
}

int NetServerTCP::processMessages()
{ 
	return messageHandler_.processMessages(); 
}

bool NetServerTCP::started()
{
	return (server_ != 0 || firstDestination_ != 0);
}

bool NetServerTCP::start(int port)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	IPaddress ip;
	if(SDLNet_ResolveHost(&ip,NULL,port)==-1)
	{
		return false;
	}

	// we seem to be able to open the same port
	// multiple times!!!
	// This is fixed as the server info port catches it
	server_=SDLNet_TCP_Open(&ip);
	if (!server_)
	{
		return false;
	}
	NetBufferUtil::setBlockingIO(server_);
	SDLNet_TCP_AddSocket(sockSet_, server_);

	return true;
}

void NetServerTCP::stop()
{
	disconnectAllClients();
}

bool NetServerTCP::connect(const char *hostName, int portNo)
{
	if(SDLNet_Init()==-1)
	{
		return false;
	}

	IPaddress ip;
	if(SDLNet_ResolveHost(&ip,(char *) hostName,portNo)==-1)
	{
		return false;
	}

	TCPsocket client=SDLNet_TCP_Open(&ip);
	if (!client)
	{
		return false;
	}
	NetBufferUtil::setBlockingIO(client);

	addClient(client);
	return true;
}

int NetServerTCP::threadFunc(void *param)
{
	NetServerTCP *netServer = (NetServerTCP *) param;

	Clock netClock;
	for (;;)
	{
		netClock.getTimeDifference();

		if (netServer->server_) 
		{
			netServer->pollIncoming();
		}
		if (netServer->checkDeleted_)
		{
			netServer->checkDeleted_ = false;
			netServer->pollDeleted();
		}

		float timeDiff = netClock.getTimeDifference();
		if (timeDiff > 1.0f)
		{
			Logger::log(S3D::formatStringBuffer("Warning: Net loop took %.2f seconds, server", 
				timeDiff));
		}

		SDL_Delay(100);
	}

	return 0;
}

bool NetServerTCP::pollIncoming()
{
	DIALOG_ASSERT(sockSet_ && server_);
	int numready = SDLNet_CheckSockets(sockSet_, 10);
	if (numready == -1) return false;
	if (numready == 0) return true;

	if(SDLNet_SocketReady(server_))
	{
		TCPsocket sock = SDLNet_TCP_Accept(server_);
		if (sock)
		{
			NetBufferUtil::setBlockingIO(sock);
			addClient(sock);
			getConnects()++;
		}
	}

	return true;
}

bool NetServerTCP::pollDeleted()
{
	SDL_LockMutex(setMutex_);
	std::list<unsigned int> remove;
	std::map<unsigned int, NetServerTCPRead *>::iterator itor;
	for (itor = connections_.begin();
		itor != connections_.end();
		++itor)
	{
		NetServerTCPRead *serverRead = (*itor).second;
		unsigned int id = (*itor).first;
		if (serverRead->getDisconnect())
		{
			delete serverRead;
			remove.push_back(id);
		}
	}
	std::list<unsigned int>::iterator itor2;
	for (itor2 = remove.begin();
		itor2 != remove.end();
		++itor2)
	{
		unsigned int id = (*itor2);
		connections_.erase(id);
	}
	SDL_UnlockMutex(setMutex_);
	return true;
}

void NetServerTCP::addClient(TCPsocket client)
{
	// Calculate the current client id
	// Mutex protect incase addClient is called from different threads
	// (unlikely)
	SDL_LockMutex(setMutex_);
	if (++lastId_ == 0) ++lastId_;
	unsigned int currentId = lastId_;
	SDL_UnlockMutex(setMutex_);

	// Create the thread to read this socket
	NetServerTCPRead *serverRead = new NetServerTCPRead(
		currentId, client, protocol_, &messageHandler_, &checkDeleted_);

	// Add this to the collection of sockets (connections)
	SDL_LockMutex(setMutex_);
	connections_[currentId] = serverRead;
	firstDestination_ = currentId;
	SDL_UnlockMutex(setMutex_);

	// Start the sockets
	serverRead->start();
}

void NetServerTCP::disconnectAllClients()
{
	SDL_LockMutex(setMutex_);
	std::map<unsigned int, NetServerTCPRead *>::iterator itor;
	for (itor = connections_.begin();
		itor != connections_.end();
		++itor)
	{
		unsigned int id = (*itor).first;
		disconnectClient(id);
	}
	SDL_UnlockMutex(setMutex_);
}

void NetServerTCP::disconnectClient(NetBuffer &buffer, unsigned int dest)
{
	disconnectClient(dest);
}

void NetServerTCP::disconnectClient(unsigned int dest)
{
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::DisconnectMessage, 
				dest, getIpAddress(dest));

	// Add the message to the list of out going
	sendMessage(dest, message);
}

void NetServerTCP::sendMessageServer(NetBuffer &buffer, 
	unsigned int flags)
{
	sendMessageDest(buffer, firstDestination_, flags);
}

void NetServerTCP::sendMessageDest(NetBuffer &buffer, 
	unsigned int destination, unsigned int flags)
							
{
	// Get a new buffer from the pool
	NetMessage *message = NetMessagePool::instance()->
		getFromPool(NetMessage::NoMessage, 
				destination, getIpAddress(destination), flags);

	// Add message to new buffer
	message->getBuffer().allocate(buffer.getBufferUsed());
	memcpy(message->getBuffer().getBuffer(), 
		buffer.getBuffer(), buffer.getBufferUsed());
	message->getBuffer().setBufferUsed(buffer.getBufferUsed());

	// Send Mesage
	sendMessage(destination, message);
}

void NetServerTCP::sendMessage(unsigned int client, NetMessage *message)
{
	// Find the client
	SDL_LockMutex(setMutex_);
	std::map<unsigned int, NetServerTCPRead *>::iterator itor = 
		connections_.find(client);
	if (itor != connections_.end()) 
	{
		// Add the message to the list of out going
		NetServerTCPRead *serverRead = (*itor).second;
		serverRead->addMessage(message);
	}
	else
	{
		NetMessagePool::instance()->addToPool(message);
		Logger::log(S3D::formatStringBuffer("Unknown sendMessage destination %u",
			client));
	}
	SDL_UnlockMutex(setMutex_);
}

unsigned int NetServerTCP::getIpAddress(unsigned int destination)
{
	unsigned int addr = 0;
	SDL_LockMutex(setMutex_);
	std::map<unsigned int, NetServerTCPRead *>::iterator itor = 
		connections_.find(destination);
	if (itor != connections_.end()) 
	{
		NetServerTCPRead *read = (*itor).second;
		addr = read->getIpAddress();
	}
	SDL_UnlockMutex(setMutex_);

	return addr;
}

