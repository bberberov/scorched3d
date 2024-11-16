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

#if !defined(__INCLUDE_NetServerTCPProtocolh_INCLUDE__)
#define __INCLUDE_NetServerTCPProtocolh_INCLUDE__

#include <net/NetMessage.h>

class NetServerTCPProtocol
{
public:
	NetServerTCPProtocol();
	virtual ~NetServerTCPProtocol();

	virtual bool sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id) = 0;
	virtual NetMessage *readBuffer(TCPsocket socket, unsigned int id) = 0;
};

class NetServerTCPScorchedProtocol : public NetServerTCPProtocol
{
public:
	NetServerTCPScorchedProtocol();
	virtual ~NetServerTCPScorchedProtocol();

	virtual bool sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id);
	virtual NetMessage *readBuffer(TCPsocket socket, unsigned int id);
};

class NetServerHTTPProtocolSend : public NetServerTCPProtocol
{
public:
	NetServerHTTPProtocolSend();
	virtual ~NetServerHTTPProtocolSend();

	virtual bool sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id);
	virtual NetMessage *readBuffer(TCPsocket socket, unsigned int id);
};

class NetServerHTTPProtocolRecv : public NetServerTCPProtocol
{
public:
	NetServerHTTPProtocolRecv();
	virtual ~NetServerHTTPProtocolRecv();

	virtual bool sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id);
	virtual NetMessage *readBuffer(TCPsocket socket, unsigned int id);
};

#endif
