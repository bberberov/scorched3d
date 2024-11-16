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

#ifndef _WIN32
#include <fcntl.h>
#define SOCKET	int
#include <sys/socket.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <net/NetBufferUtil.h>

// HACK HACK HACK
// This code has been ripped from the SDL_net library
// The library always sets the sockets to be non-blocking
// this is NOT what we want as we use threads and do not
// care.
// This seems to be the only way to set blocking IO back on.

struct _TCPsocket {
	int ready;
	SOCKET channel;
	IPaddress remoteAddress;
	IPaddress localAddress;
	int sflag;
};

bool NetBufferUtil::getLocalIPAddress(TCPsocket &so, std::string &result)
{
	struct sockaddr saddr;
	socklen_t saddrlen = sizeof(saddr);
	if(getsockname(so->channel, &saddr, &saddrlen) < 0) return false;

	char addressName[256];
	if (getnameinfo(&saddr, saddrlen,
		addressName, 256,
		NULL, 0,
		NI_NUMERICHOST | NI_NUMERICSERV) != 0) 
	{
		return false;
	}
	result = addressName;
	return true;
}

void NetBufferUtil::setBlockingIO(TCPsocket &so)
{
#ifdef O_NONBLOCK
	/* Set the socket to blocking mode for accept() */
	fcntl(so->channel, F_SETFL, 0);
#else
#ifdef WIN32
	{
		/* passing a zero value, socket mode set blocking */
		unsigned long mode = 0;
		ioctlsocket (so->channel, FIONBIO, &mode);
	}
#else
#warning How do we set blocking mode on other operating systems?
#endif /* WIN32 */
#endif /* O_NONBLOCK */
}
