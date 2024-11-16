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

#include <net/NetBufferPool.h>

NetBufferPool *NetBufferPool::instance_ = 0;

NetBufferPool *NetBufferPool::instance()
{
	if (!instance_)
	{
		instance_ = new NetBufferPool;
	}
	return instance_;
}

NetBufferPool::NetBufferPool() : messagePoolMutex_(0)
{
	messagePoolMutex_ = SDL_CreateMutex();
}

NetBufferPool::~NetBufferPool()
{
	SDL_DestroyMutex(messagePoolMutex_);
}

void NetBufferPool::addToPool(NetBuffer *message)
{
	SDL_LockMutex(messagePoolMutex_);
	messagePool_.push_back(message);
	SDL_UnlockMutex(messagePoolMutex_);
}

NetBuffer *NetBufferPool::getFromPool()
{
	SDL_LockMutex(messagePoolMutex_);

	NetBuffer *result = 0;
	if (messagePool_.empty())
	{
		result = new NetBuffer;
	}
	else
	{
		result = messagePool_.front();
		messagePool_.pop_front();
	}
	result->reset();
	SDL_UnlockMutex(messagePoolMutex_);

	return result;
}
