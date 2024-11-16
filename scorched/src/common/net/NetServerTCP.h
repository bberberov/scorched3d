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

#ifndef __INCLUDE_netServer_h_INCLUDE__
#define __INCLUDE_netServer_h_INCLUDE__

#include <net/NetInterface.h>
#include <net/NetServerTCPProtocol.h>
#include <net/NetServerTCPRead.h>
#include <list>
#include <map>

class NetServerTCP : public NetInterface
{
public:
	NetServerTCP(NetServerTCPProtocol *protocol);
	virtual ~NetServerTCP();

	virtual bool started();
	virtual bool connect(const char *hostName, int portNo);
	virtual bool start(int portNo);
	virtual void stop();

	virtual int processMessages();
	virtual void setMessageHandler(NetMessageHandlerI *handler);

	virtual void disconnectAllClients();
	virtual void disconnectClient(unsigned int client);
	virtual void disconnectClient(NetBuffer &buffer, 
		unsigned int client);
	virtual void sendMessageServer(NetBuffer &buffer, 
		unsigned int flags = 0);
	virtual void sendMessageDest(NetBuffer &buffer, 
		unsigned int destination, unsigned int flags = 0);

protected:
	NetServerTCPProtocol *protocol_;
	TCPsocket server_;
	unsigned int lastId_;
	unsigned int firstDestination_;
	SDLNet_SocketSet sockSet_;
	std::map<unsigned int, NetServerTCPRead *> connections_;
	SDL_mutex *setMutex_;
	NetMessageHandler messageHandler_;
	bool checkDeleted_;

	static int threadFunc(void *);

	bool pollIncoming();
	bool pollDeleted();
	void addClient(TCPsocket client);
	void sendMessage(unsigned int client, NetMessage *message);
	unsigned int getIpAddress(unsigned int destination);

private:

	NetServerTCP(const NetServerTCP &);
	const NetServerTCP & operator=(const NetServerTCP &);

};

#endif // __INCLUDE_netServer_h_INCLUDE__

