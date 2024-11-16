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

#include <XML/XMLStringBuffer.h>
#include <serverbrowser/ServerBrowserRefresh.h>
#include <serverbrowser/ServerBrowser.h>
#include <common/Defines.h>
#include <time.h>

ServerBrowserRefreshEntry::ServerBrowserRefreshEntry() :
	retries_(0), sentTime_(0), recieved_(0)
{
}

ServerBrowserRefreshEntry::ServerBrowserRefreshEntry(
	const char *address,
	int position) : 
	address_(address),
	position_(position),
	retries_(0), sentTime_(0), recieved_(0)
{
}

ServerBrowserRefresh::ServerBrowserRefresh(ServerBrowserServerList &list) :
	list_(list), 
	cancel_(false)
{
	recvPacket_ = SDLNet_AllocPacket(10000);
	sendPacketStatus_ = SDLNet_AllocPacket(20);
	sendPacketStatus_->len = 7;
	memcpy(sendPacketStatus_->data, "status", 7);
	sendPacketPlayers_ = SDLNet_AllocPacket(20);
	sendPacketPlayers_->len = 8;
	memcpy(sendPacketPlayers_->data, "players", 8);
}

ServerBrowserRefresh::~ServerBrowserRefresh()
{
}

void ServerBrowserRefresh::refreshList()
{
	for (int i=0; i<list_.getNoEntries(); i++)
	{
		const char *address = list_.getEntryValue(i, "address");
		if (address && address[0])
		{
			ServerBrowserRefreshEntry entry(address, i);
			refreshEntries_.push_back(entry);
		}
	}

	while ((!refreshEntries_.empty() || !entryMap_.empty()) && !cancel_)
	{
		time_t theTime = time(0);

		// Add a number of new entries
		for (int i=(int) entryMap_.size(); i<20 && !refreshEntries_.empty(); i++)
		{
			ServerBrowserRefreshEntry entry = refreshEntries_.front();
			refreshEntries_.pop_front();

			sendNextEntry(entry, theTime);
		}
		SDL_Delay(1000);

		// Process all new messages
		processMessages(theTime);
	}

	// Tidy incase of cancel
	std::map<UDPsocket, ServerBrowserRefreshEntry>::iterator cleanitor;
	for (cleanitor = entryMap_.begin();
		cleanitor != entryMap_.end();
		++cleanitor)
	{
		UDPsocket socket = (*cleanitor).first;
		SDLNet_UDP_Close(socket);
	}
	refreshEntries_.clear();
	entryMap_.clear();
}

void ServerBrowserRefresh::sendNextEntry(
	ServerBrowserRefreshEntry &entry, time_t theTime)
{
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "%s", entry.address_.c_str());
	char *port = strchr(buffer, ':');
	if (!port) return;
	*port = '\0';
	port++;

	IPaddress address;
	if (SDLNet_ResolveHost(&address, buffer, atoi(port)+1) != 0) return;

	// Connect to the client
	UDPsocket udpsock = SDLNet_UDP_Open(0);
	if (!udpsock) return;

	int chan = SDLNet_UDP_Bind(udpsock, -1, &address);
	if (chan == -1) return;

	// Add the client to the map of currently processed clients
	entry.retries_ ++;
	entry.sentTime_ = (unsigned int) theTime;
	entry.recieved_ = 0;
	entryMap_[udpsock] = entry;

	// Send the request for info
	SDLNet_UDP_Send(udpsock, chan, sendPacketStatus_);
	SDLNet_UDP_Send(udpsock, chan, sendPacketPlayers_);
}

void ServerBrowserRefresh::processMessages(time_t theTime)
{
	std::list<UDPsocket> finished;
	std::map<UDPsocket, ServerBrowserRefreshEntry>::iterator itor;
	for (itor = entryMap_.begin();
		itor != entryMap_.end();
		++itor)
	{
		UDPsocket socket = (*itor).first;
		ServerBrowserRefreshEntry &entry = (*itor).second;

		if (theTime - entry.sentTime_ > 5)
		{
			if (entry.retries_ < 3) refreshEntries_.push_back(entry);
			finished.push_back(socket);
		}
		else
		{
			while (SDLNet_UDP_Recv(socket, recvPacket_))
			{
				processMessage(recvPacket_, entry);
				entry.recieved_ ++;

				if (entry.recieved_ == 2) finished.push_back(socket);
			}
		}
	}

	while (!finished.empty())
	{
		UDPsocket socket = finished.front();
		finished.pop_front();

		entryMap_.erase(socket);
		SDLNet_UDP_Close(socket);
	}
}

void ServerBrowserRefresh::processMessage(UDPpacket *packet, ServerBrowserRefreshEntry &entry)
{
	const char *buffer = (char *) packet->data;
	unsigned int len = packet->len - 1;

	// Parse the buffer
	XMLStringBuffer xmlBuffer;
	if (xmlBuffer.create(buffer, len))
	{
		// Itterate all of the keys in the buffer
		std::list<XMLNode *>::iterator childrenItor;
		std::list<XMLNode *> &children = xmlBuffer.getRootNode()->getParameters();
		for (childrenItor = children.begin();
			childrenItor != children.end();
			++childrenItor)
		{
			XMLNode *currentNode = (*childrenItor);
			list_.addEntryValue(entry.position_,
				currentNode->getName(),
				currentNode->getContent());
		}
	}
}

