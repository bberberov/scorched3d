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

#include <webserver/ServerWebServer.h>

ServerWebServerQueueEntry::ServerWebServerQueueEntry(
	unsigned int destinationId,
	unsigned int sid,
	const char *url,
	ServerWebServerI *handler,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts) :
	destinationId_(destinationId),
	sid_(sid), 
	request_(url, fields, parts),
	handler_(handler)
{
}

ServerWebServerQueueEntry::~ServerWebServerQueueEntry()
{
	delete handler_;
}

ServerWebServerQueue::ServerWebServerQueue() 
{
	queueMutex_ = SDL_CreateMutex();
}

ServerWebServerQueue::~ServerWebServerQueue()
{
}

void ServerWebServerQueue::addEntry(ServerWebServerQueueEntry *entry)
{
	SDL_LockMutex(queueMutex_);
	entries_.push_back(entry);
	SDL_UnlockMutex(queueMutex_);
}

ServerWebServerQueueEntry *ServerWebServerQueue::getEntry()
{
	ServerWebServerQueueEntry *result = 0;
	SDL_LockMutex(queueMutex_);
	if (!entries_.empty())
	{
		result = entries_.front();
		entries_.pop_front();
	}
	SDL_UnlockMutex(queueMutex_);
	return result;
}

void ServerWebServerQueue::removeEntry(unsigned int destinationId)
{
	SDL_LockMutex(queueMutex_);
	std::list<ServerWebServerQueueEntry *>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		ServerWebServerQueueEntry *entry = *itor;
		if (entry->getDestinationId() == destinationId)
		{
			entries_.erase(itor);
			delete entry;
			break;
		}
	}
	SDL_UnlockMutex(queueMutex_);
}

bool ServerWebServerQueue::hasEntry(unsigned int destinationId)
{
	bool result = false;

	SDL_LockMutex(queueMutex_);
	std::list<ServerWebServerQueueEntry *>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		ServerWebServerQueueEntry *entry = *itor;
		if (entry->getDestinationId() == destinationId)
		{
			result = true;
			break;
		}
	}
	SDL_UnlockMutex(queueMutex_);

	return result;
}
