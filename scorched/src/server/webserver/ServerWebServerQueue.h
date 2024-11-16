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

#if !defined(__INCLUDE_ServerWebServerQueueh_INCLUDE__)
#define __INCLUDE_ServerWebServerQueueh_INCLUDE__

#include <SDL/SDL.h>
#include <webserver/ServerWebServerI.h>
#include <list>

class ServerWebServerQueueEntry
{
public:
	ServerWebServerQueueEntry(
		unsigned int destinationId,
		unsigned int sid,
		const char *url,
		ServerWebServerI *handler,
		std::map<std::string, std::string> &fields,
		std::map<std::string, NetMessage *> &parts);
	~ServerWebServerQueueEntry();

	unsigned int getDestinationId() { return destinationId_; }
	unsigned int getSid() { return sid_; }
	ServerWebServerI *getHandler() { return handler_; }
	ServerWebServerIRequest &getRequest() { return request_; }

protected:
	ServerWebServerI *handler_;
	unsigned int destinationId_;
	unsigned int sid_;
	ServerWebServerIRequest request_;
};

class ServerWebServerQueue
{
public:
	ServerWebServerQueue();
	virtual ~ServerWebServerQueue();

	void addEntry(ServerWebServerQueueEntry *entry);
	ServerWebServerQueueEntry *getEntry();

	void removeEntry(unsigned int destinationId);
	bool hasEntry(unsigned int destinationId);
	
protected:
	SDL_mutex *queueMutex_;
	std::list<ServerWebServerQueueEntry *> entries_;
};

#endif
