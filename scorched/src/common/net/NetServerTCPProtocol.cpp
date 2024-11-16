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

#include <net/NetServerTCPProtocol.h>
#include <net/NetServerTCPRead.h>
#include <net/NetMessagePool.h>
#include <common/Logger.h>

NetServerTCPProtocol::NetServerTCPProtocol()
{
}

NetServerTCPProtocol::~NetServerTCPProtocol()
{
}

NetServerTCPScorchedProtocol::NetServerTCPScorchedProtocol()
{
}

NetServerTCPScorchedProtocol::~NetServerTCPScorchedProtocol()
{
}

bool NetServerTCPScorchedProtocol::sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id)
{
	Uint32 len = buffer.getBufferUsed();
	Uint32 netlen=0;
	SDLNet_Write32(len, &netlen);

	// send the length of the string
	int result = SDLNet_TCP_Send(socket, &netlen, sizeof(netlen));
	if(result<(int) sizeof(netlen))
	{
		Logger::log(S3D::formatStringBuffer("Failed to send buffer length. Sent %i of %i.",
			result, sizeof(netlen)));
		return false;
	}
	
	// send the buffer
	result = SDLNet_TCP_Send(socket,buffer.getBuffer(),len);
	if(result<int(len))
	{
		Logger::log(S3D::formatStringBuffer("Failed to send buffer. Sent %i of %i.",
			result, int(len)));
		return false;
	}
	NetInterface::getBytesOut() += len;
	
	// return the length sent
	return true;
}

static bool realSDLNet_TCP_Recv(TCPsocket socket, char *dest, int len)
{
	int result = 0;
	while (len > 0)
	{
		int recv = SDLNet_TCP_Recv(socket, &dest[result], len);
		if (recv <= 0) 
		{
			return false;
		}

		result += recv;
		len -= recv;

		if (len > 0)
		{
			//Logger::log( "Partial read, %i/%i %s", recv, len + recv, 
				//SDLNet_GetError());
			SDL_Delay(10);
		}
	}	

	return true;
}

NetMessage *NetServerTCPScorchedProtocol::readBuffer(TCPsocket socket, unsigned int id)
{
	// receive the length of the string message
	char lenbuf[4];
	if (!realSDLNet_TCP_Recv(socket, lenbuf, 4))
	{
		Logger::log( "Socket closed.");
		return 0;
	}

	// swap byte order to our local order
	Uint32 len = SDLNet_Read32(lenbuf);
	
	// check if anything is strange, like a zero length buffer
	if(len == 0)
	{
		Logger::log( "Zero length buffer recieved.");
		return 0;
	}

	// Cannot recieve a message large than .5 MB
	if (len > 5000000)
	{
		Logger::log(S3D::formatStringBuffer("Buffer was too large to recieve.  Size %i.",
			len));
		return 0;
	}

	// allocate the buffer memory
	NetMessage *buffer = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage, 
				id,
				NetServerTCPRead::getIpAddressFromSocket(socket));
	buffer->getBuffer().allocate(len);
	buffer->getBuffer().setBufferUsed(len);

	// get the string buffer over the socket
	if (!realSDLNet_TCP_Recv(socket, 
		buffer->getBuffer().getBuffer(),
		len))
	{
		Logger::log( "Read failed for buffer");
		NetMessagePool::instance()->addToPool(buffer);
		return 0;
	}
	NetInterface::getBytesIn() += len;

	// return the new buffer
	return buffer;
}


NetServerHTTPProtocolSend::NetServerHTTPProtocolSend()
{
}

NetServerHTTPProtocolSend::~NetServerHTTPProtocolSend()
{
}

bool NetServerHTTPProtocolSend::sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id)
{
	Uint32 len = buffer.getBufferUsed();
	
	// send the buffer
	int result = SDLNet_TCP_Send(socket,buffer.getBuffer(),len);
	if(result<int(len))
	{
		Logger::log(S3D::formatStringBuffer("Failed to send HTTP buffer. Sent %i of %i.",
			result, int(len)));
		return false;
	}
	NetInterface::getBytesOut() += len;
	
	// return the length sent
	return true;
}

NetMessage *NetServerHTTPProtocolSend::readBuffer(TCPsocket socket, unsigned int id)
{
	// allocate the buffer memory
	NetMessage *netBuffer = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage,
				id,
				NetServerTCPRead::getIpAddressFromSocket(socket));
	netBuffer->getBuffer().reset();

	// get the string buffer over the socket
	Uint32 len = 0;
	char buffer[1];
	for (;;)
	{
		int recv = SDLNet_TCP_Recv(socket, buffer, 1);
		if (recv <= 0) 
		{
			// For HTTP the socket being closed signifies the end
			// of the transmission and is probably not an error!
			if (len == 0)
			{
				// If the len is zero then we have been disconnected
				NetMessagePool::instance()->addToPool(netBuffer);
				return 0;
			}
			else break;
		}

		netBuffer->getBuffer().addDataToBuffer(buffer, 1);
		len += 1;
	}
	NetInterface::getBytesIn() += len;

	// return the new buffer
	return netBuffer;
}

NetServerHTTPProtocolRecv::NetServerHTTPProtocolRecv()
{
}

NetServerHTTPProtocolRecv::~NetServerHTTPProtocolRecv()
{
}

bool NetServerHTTPProtocolRecv::sendBuffer(NetBuffer &buffer, TCPsocket socket, unsigned int id)
{
	Uint32 len = buffer.getBufferUsed();
	
	// send the buffer
	int result = SDLNet_TCP_Send(socket,buffer.getBuffer(),len);
	if(result<int(len))
	{
		Logger::log(S3D::formatStringBuffer("Failed to send HTTP buffer. Sent %i of %i.",
			result, int(len)));
		return false;
	}
	NetInterface::getBytesOut() += len;
	
	// return the length sent
	return true;
}

NetMessage *NetServerHTTPProtocolRecv::readBuffer(TCPsocket socket, unsigned int id)
{
	// allocate the buffer memory
	NetMessage *netBuffer = NetMessagePool::instance()->
		getFromPool(NetMessage::BufferMessage,
				id,
				NetServerTCPRead::getIpAddressFromSocket(socket));
	netBuffer->getBuffer().reset();

	// get the string buffer over the socket
	unsigned int len = 0;
	unsigned int dataLen = 0;
	char buffer[1];
	for (;;)
	{
		int recv = SDLNet_TCP_Recv(socket, buffer, 1);
		if (recv <= 0) 
		{
			// For HTTP the socket being closed signifies the end
			// of the transmission and is probably not an error!
			if (len == 0)
			{
				// If the len is zero then we have been disconnected
				NetMessagePool::instance()->addToPool(netBuffer);
				return 0;
			}
			else break;
		}

		netBuffer->getBuffer().addDataToBuffer(buffer, 1);
		len += 1;
		
		// Check for the end of the HTTP header
		if (len > 4 && dataLen == 0)
		{
			if (netBuffer->getBuffer().getBuffer()[len - 4] == '\r' &&
				netBuffer->getBuffer().getBuffer()[len - 3] == '\n' &&
				netBuffer->getBuffer().getBuffer()[len - 2] == '\r' &&
				netBuffer->getBuffer().getBuffer()[len - 1] == '\n')
			{
				// We have now found the end of the http header
				// check if there is any data as well
				netBuffer->getBuffer().getBuffer()[len - 4] = '\0';
				char *length = strstr(netBuffer->getBuffer().getBuffer(), "Content-Length: ");
				if (length)
				{
					length += 16;
					int dl = atoi(length);
					dataLen = len + dl;
				}
				else
				{
					// No data, so end at current len
					dataLen = len;
				}
				netBuffer->getBuffer().getBuffer()[len - 4] = '\r';
			}
			
		}
		
		if (len >= dataLen && dataLen != 0) break;
	}
	NetInterface::getBytesIn() += len;

	// return the new buffer
	return netBuffer;
}
