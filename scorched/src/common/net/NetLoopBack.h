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

#if !defined(__INCLUDE_NetLoopBackh_INCLUDE__)
#define __INCLUDE_NetLoopBackh_INCLUDE__

#include <net/NetInterface.h>
#include <net/NetMessageHandler.h>

class NetLoopBack : public NetInterface
{
public:
	NetLoopBack(bool server);
	virtual ~NetLoopBack();

	virtual bool started();
	virtual bool connect(const char *hostName, int portNo);
	virtual bool start(int portNo) { return true; }
	virtual void stop() { }

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
	bool started_;
	bool server_;
	NetMessageHandler messageHandler_;
	
	NetLoopBack *getLoopback();

	static NetLoopBack *serverLoopback_, *clientLoopback_;
};

#endif
