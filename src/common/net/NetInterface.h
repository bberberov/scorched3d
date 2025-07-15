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

#if !defined(__INCLUDE_NetInterfaceh_INCLUDE__)
#define __INCLUDE_NetInterfaceh_INCLUDE__

#include <net/NetMessageHandler.h>
#include <net/NetInterfaceFlags.h>

class NetInterface
{
public:
	NetInterface();
	virtual ~NetInterface();

	virtual bool started() = 0;
	virtual bool connect(const char *hostName, int portNo) = 0;
	virtual bool start(int portNo) = 0;
	virtual void stop() = 0;

	virtual int processMessages() = 0;
	virtual void setMessageHandler(NetMessageHandlerI *handler) = 0;

	virtual void disconnectAllClients() = 0;
	virtual void disconnectClient(unsigned int client) = 0;
	virtual void disconnectClient(NetBuffer &buffer, 
		unsigned int client) = 0;
	virtual void sendMessageServer(NetBuffer &buffer, 
		unsigned int flags = 0) = 0;
	virtual void sendMessageDest(NetBuffer &buffer, 
		unsigned int destination, unsigned int flags = 0) = 0;

	static unsigned int &getBytesIn() { return bytesIn_; }
	static unsigned int &getBytesOut() { return bytesOut_; }
	static unsigned int &getPings() { return pings_; }
	static unsigned int &getConnects() { return connects_; }
	static const char *getIpName(unsigned int ipAddress);

protected:
	static unsigned int bytesIn_;
	static unsigned int bytesOut_;
	static unsigned int pings_;
	static unsigned int connects_;
};

#endif
