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


#include <net/NetMessagePool.h>

NetMessagePool *NetMessagePool::instance_ = 0;

NetMessagePool *NetMessagePool::instance()
{
	if (!instance_)
	{
		instance_ = new NetMessagePool;
	}
	return instance_;
}

NetMessagePool::NetMessagePool() : messagePoolMutex_(0)
{
	messagePoolMutex_ = SDL_CreateMutex();
}

NetMessagePool::~NetMessagePool()
{
	SDL_DestroyMutex(messagePoolMutex_);
}

void NetMessagePool::addToPool(NetMessage *message)
{
	SDL_LockMutex(messagePoolMutex_);
	messagePool_.push_back(message);
	SDL_UnlockMutex(messagePoolMutex_);
}

NetMessage *NetMessagePool::getFromPool(NetMessage::MessageType type,
	unsigned int destinationId,
	unsigned int ipAddress,
	unsigned int flags,
	unsigned int recvTime)
{
	SDL_LockMutex(messagePoolMutex_);

	NetMessage *result = 0;
	if (messagePool_.empty())
	{
		result = new NetMessage;
	}
	else
	{
		result = messagePool_.front();
		messagePool_.pop_front();
	}
	result->setDestinationId(destinationId);
	result->setIpAddress(ipAddress);
	result->setType(type);
	result->getBuffer().reset();
	result->setFlags(flags);
	result->setRecvTime(recvTime);

	SDL_UnlockMutex(messagePoolMutex_);

	return result;
}
