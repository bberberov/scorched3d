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


#if !defined(__INCLUDE_NetMessageh_INCLUDE__)
#define __INCLUDE_NetMessageh_INCLUDE__

#include <net/NetBuffer.h>

class NetMessagePool;
class NetMessage
{
public:
	friend class NetMessagePool;

	enum MessageType
	{
		NoMessage = 0,
		ConnectMessage,
		DisconnectMessage,
		DisconnectAllMessage,
		BufferMessage,
		SentMessage
	};
	enum DisconnectFlags
	{
		UnknownDisconnect = 0,
		UserDisconnect = 1,
		KickDisconnect = 2,
		TimeoutDisconnect = 4
	};

	unsigned int getDestinationId() { return destinationId_; }
	unsigned int getIpAddress() { return ipAddress_; }
	unsigned int getFlags() { return flags_; }
	unsigned int getRecvTime() { return recvTime_; }
	MessageType getMessageType() { return type_; }
	NetBuffer &getBuffer() { return messageBuffer_; }
	void setType(MessageType type) { type_ = type; }

protected:
	NetBuffer messageBuffer_;
	MessageType type_;
	unsigned int destinationId_;
	unsigned int ipAddress_;
	unsigned int flags_;
	unsigned int recvTime_;

	void setDestinationId(unsigned int destinationId) { destinationId_ = destinationId; }
	void setIpAddress(unsigned int ipAddress) { ipAddress_ = ipAddress; }
	void setRecvTime(unsigned int recvTime) { recvTime_ = recvTime; }
	void setFlags(unsigned int flags) { flags_ = flags; }

	NetMessage(MessageType type = NetMessage::NoMessage,
			   unsigned int destinationId = 0,
			   unsigned int ipAddress = 0);
	virtual ~NetMessage();
};

#endif // __INCLUDE_NetMessageh_INCLUDE__
