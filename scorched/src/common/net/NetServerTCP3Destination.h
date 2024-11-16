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

#if !defined(__INCLUDE_NetServerTCP3Destinationh_INCLUDE__)
#define __INCLUDE_NetServerTCP3Destinationh_INCLUDE__

#include <net/NetServerTCP3Send.h>
#include <net/NetServerTCP3Recv.h>

class NetServerTCP3Destination
{
public:
	NetServerTCP3Destination(NetMessageHandler *recieveMessageHandler,
		TCPsocket socket, unsigned int destinationId);
	virtual ~NetServerTCP3Destination();

	void sendMessage(NetMessage *message);
	void printStats();

	void close(NetMessage *message);
	bool allFinished();
	bool anyFinished();

	unsigned int getIpAddress() { return ipAddress_; }

protected:
	NetServerTCP3Send send_;
	NetServerTCP3Recv recv_;
	unsigned int destinationId_, ipAddress_;
	TCPsocket socket_;
	bool running_;

	static unsigned int getIpAddressFromSocket(TCPsocket socket);
};

#endif // __INCLUDE_NetServerTCP3Destinationh_INCLUDE__
