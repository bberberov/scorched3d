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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#else
#include <errno.h>
#endif
#include <common/Logger.h>
#include <net/NetServerTCP3Coms.h>

int NetServerTCP3Coms::SDLNet_TCP_Recv_Wrapper(TCPsocket sock, void *data, int maxlen)
{
#ifdef WIN32
	WSASetLastError(0);
#else
	errno = 0;
#endif

	int result = SDLNet_TCP_Recv(sock, data, maxlen);
	if (result <= 0)
	{
#ifdef WIN32
		int wsacp = WSAGetLastError();
		if (wsacp != WSAECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Recv_Wrapper: WSA Error code %i", wsacp));
		}
#else
		int errnocp = errno;
		if (errnocp != ECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Recv_Wrapper: Error code %i", errnocp));
		}
#endif
	}

	return result;
}

int NetServerTCP3Coms::SDLNet_TCP_Send_Wrapper(TCPsocket sock, void *datap, int len)
{
#ifdef WIN32
	WSASetLastError(0);
#else
	errno = 0;
#endif

	int result = SDLNet_TCP_Send(sock, datap, len);
	if (result <= 0)
	{
#ifdef WIN32
		int wsacp = WSAGetLastError();
		if (wsacp != WSAECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Send_Wrapper: WSA Error code %i", wsacp));
		}
#else
		int errnocp = errno;
		if (errnocp != ECONNRESET)
		{
			Logger::log(S3D::formatStringBuffer(
				"SDLNet_TCP_Send_Wrapper: Error code %i", errnocp));
		}
#endif
	}

	return result;
}

bool NetServerTCP3Coms::SDLNet_TCP_Recv_Full(TCPsocket socket, char *dest, int len)
{
	int result = 0;
	while (len > 0)
	{
		int recv = SDLNet_TCP_Recv_Wrapper(socket, &dest[result], len);
		if (recv <= 0) 
		{
			return false;
		}

		result += recv;
		len -= recv;

		if (len > 0) SDL_Delay(10);
	}	

	return true;
}
