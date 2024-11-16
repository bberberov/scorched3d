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

#include <serverbrowser/ServerBrowserCollect.h>
#include <serverbrowser/ServerBrowser.h>
#include <XML/XMLStringBuffer.h>
#include <XML/XMLFile.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>
#include <common/OptionsMasterListServer.h>
#include <time.h>

ServerBrowserCollect::ServerBrowserCollect(ServerBrowserServerList &list) :
	list_(list),
	netServer_(new NetServerHTTPProtocolSend), 
	complete_(false), 
	cancel_(false)
{
	// All messages will come to this class
	netServer_.setMessageHandler(this);
	recvPacket_ = SDLNet_AllocPacket(10000);
	sendPacket_ = SDLNet_AllocPacket(20);
	sendPacket_->len = 5;
	memcpy(sendPacket_->data, "ping", 5);
}

ServerBrowserCollect::~ServerBrowserCollect()
{
}

bool ServerBrowserCollect::fetchServerList(
	const char *masterListServer,
	const char *masterListServerURI)
{
	// Create the message that will be sent to the master server
	std::string buffer = S3D::formatStringBuffer(
		"GET %s/servers.php HTTP/1.0\r\n"
		"User-Agent: Scorched3D\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"\r\n"
		"\r\n",
		masterListServerURI,
		masterListServer);
	sendNetBuffer_.reset();
	sendNetBuffer_.addDataToBuffer(buffer.c_str(), (int) buffer.size()); // Note no null

	complete_ = false;

	list_.clear();

	// Connect to the master server
	if (!netServer_.connect(masterListServer, 80))
	{
		return false;
	}

	// Send the master server request
	netServer_.sendMessageServer(sendNetBuffer_);
	
	const int WaitTime =
		OptionsMasterListServer::instance()->getMasterListServerTimeout();
	// Wait WaitTime seconds for the result
	time_t startTime, currentTime;
	startTime = currentTime = time(0);
	while ((currentTime - startTime < WaitTime) && !cancel_)
	{
		// Process any waiting messages
		netServer_.processMessages();

		// Check if the messages have made us complete
		if (complete_)
		{
			return (list_.getNoEntries() > 0);
		}

		SDL_Delay(100);
		currentTime = time(0);
	}

	// Ensure that we only have one open connection to the server
	netServer_.disconnectAllClients();

	// Process any waiting messages caused by the disconnect all
	netServer_.processMessages();

	complete_ = true;
	return false;
}

bool ServerBrowserCollect::fetchLANList()
{
	complete_ = false;

	DIALOG_ASSERT(SDLNet_Init()!=-1);

	list_.clear();
	
	SDLNet_ResolveHost(&(sendPacket_->address), "255.255.255.255", S3D::ScorchedPort + 1);
	
	// Connect to the client
	UDPsocket udpsock = SDLNet_UDP_Open(0);
	if (!udpsock) return false;
	
	// Send the request for info
	SDLNet_UDP_Send(udpsock, -1, sendPacket_);

	const int WaitTime =
		OptionsMasterListServer::instance()->getMasterListServerTimeout();

	// Accept the results
	time_t startTime = time(0);
	for (;;)
	{
		SDL_Delay(100);
		if (SDLNet_UDP_Recv(udpsock, recvPacket_))
		{
			unsigned int addr = SDLNet_Read32(&recvPacket_->address.host);
			unsigned int port = SDLNet_Read16(&recvPacket_->address.port);

			// Get the name attribute
			ServerBrowserEntry newEntry;
			char hostName[256];
			snprintf(hostName,
					256,
					"%s:%i",
					NetInterface::getIpName(addr),
					(port - 1));
			newEntry.addAttribute("address", hostName);
			
			// Add the new and its attributes
			list_.addEntry(newEntry);
		}
		
		time_t theTime = time(0);
		if ((theTime - startTime > WaitTime) || cancel_) break;
	}
	
	SDLNet_UDP_Close(udpsock);
	complete_ = true;

	return true;
}

void ServerBrowserCollect::processMessage(NetMessage &message)
{
	// We have received a reply from the web server
	if (message.getMessageType() == NetMessage::ConnectMessage)
	{
	}
	else if (message.getMessageType() == NetMessage::DisconnectMessage)
	{
		complete_ = true;
	}
	else if (message.getMessageType() == NetMessage::BufferMessage)
	{
		// Add the null to the message
		message.getBuffer().addDataToBuffer("\0", 1);
		const char *buffer = message.getBuffer().getBuffer();

		// Find the start of the XML
		while (*buffer != '<' && *buffer != '\0') buffer++;
		unsigned int len = (unsigned int) strlen(buffer);

		// Parse the buffer
		if (*buffer == '<')
		{
			XMLStringBuffer xmlBuffer;
			if (xmlBuffer.create(buffer, len))
			{
				// Itterate all of the keys in the buffer
				std::list<XMLNode *>::iterator childrenItor;
				std::list<XMLNode *> &children = xmlBuffer.getRootNode()->getChildren();
				for (childrenItor = children.begin();
					childrenItor != children.end();
					++childrenItor)
				{
					// Find each server node
					XMLNode *currentNode = (*childrenItor);
					if (0 == strcmp(currentNode->getName(), "server"))
					{
						ServerBrowserEntry newEntry;

						// Get the name attribute
						std::string address;
						if (currentNode->getNamedChild("name", address, false))
						{
							newEntry.addAttribute("address", address.c_str());
						}
						
						// Get the official attribute
						std::string type;
						if (currentNode->getNamedChild("official", type, false))
						{
							newEntry.addAttribute("type", "official");
						}
						if (currentNode->getNamedChild("mod", type, false))
						{
							newEntry.addAttribute("type", "mod");
						}
						
						// Add the new and its attributes
						list_.addEntry(newEntry);
					}
				}
			}
		}
	}
}

bool ServerBrowserCollect::fetchFavoritesList()
{
	list_.clear();
	std::set<std::string> favs = getFavourites();
	std::set<std::string>::iterator itor;
	for (itor = favs.begin();
		itor != favs.end();
		++itor)
	{
		const char *fav = (*itor).c_str();

		ServerBrowserEntry newEntry;
		newEntry.addAttribute("address", fav);

		// Add the new and its attributes
		list_.addEntry(newEntry);
	}

	return true;
}

std::set<std::string> ServerBrowserCollect::getFavourites()
{
	std::set<std::string> result;
	std::string filePath = S3D::getSettingsFile("netfavourites.xml");

	XMLFile file;
	if (!file.readFile(filePath))
	{
		S3D::dialogMessage("Scorched3D", S3D::formatStringBuffer(
			"ERROR: Failed to parse file \"%s\"\n"
			"%s",
			filePath.c_str(),
			file.getParserError()));
		return result;
	}

	// return true for an empty file
	if (!file.getRootNode()) return result;

	// Itterate all of the addresses in the file
	std::list<XMLNode *>::iterator childrenItor;
	for (childrenItor = file.getRootNode()->getChildren().begin();
		childrenItor != file.getRootNode()->getChildren().end();
		++childrenItor)
	{
		XMLNode *currentNode = (*childrenItor);
		result.insert(currentNode->getContent());
	}

	return result;
}

void ServerBrowserCollect::setFavourites(std::set<std::string> &favs)
{
	XMLNode favouritesNode("favourites");

	std::set<std::string>::iterator itor;
	for (itor = favs.begin();
		itor != favs.end();
		++itor)
	{
		const char *fav = (*itor).c_str();
		favouritesNode.addChild(new XMLNode("favourite", fav));
	}

	std::string filePath = S3D::getSettingsFile("netfavourites.xml");
	favouritesNode.writeToFile(filePath);
}

