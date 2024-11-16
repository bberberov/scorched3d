////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <webserver/ServerWebAppletHandler.h>
#include <webserver/ServerWebServerUtil.h>
#include <webserver/ServerWebServer.h>
#include <server/ServerChannelManager.h>
#include <server/ServerAdminCommon.h>
#include <server/ScorchedServer.h>
#include <XML/XMLNode.h>

bool ServerWebAppletHandler::AppletFileHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Get file
	std::string file = 
		S3D::getDataFile(
			S3D::formatStringBuffer("data/html/server/binary/%s", request.getUrl()));

	// Read file contents
	std::string contents;
	FILE *in = fopen(file.c_str(), "rb");
	if (!in) return false;
	int read = 0;
	char buffer[256];
	while (read = fread(buffer, 1, 256, in))
	{
		contents.append(buffer, read);
	}
	fclose(in);

	// Return file
	unsigned int dataSize = contents.size();
	text.append(S3D::formatStringBuffer(
		"HTTP/1.1 200 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Length: %u\r\n"
		"Content-Type: application/octet-stream\r\n"
		"Connection: Close\r\n"
		"\r\n", dataSize));
	text.append(contents.data(), dataSize);

	return true;
}

bool ServerWebAppletHandler::AppletHtmlHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "applet.html", request.getFields(), text);
}

bool ServerWebAppletHandler::AppletActionHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action)
	{
		// Check which action we need to perform
		if (0 == strcmp(action, "chat"))
		{
			// Add a new chat message
			const char *text = ServerWebServerUtil::getField(request.getFields(), "text");
			const char *channel = ServerWebServerUtil::getField(request.getFields(), "channel");
			if (text && channel && request.getSession())
			{
				ServerAdminCommon::adminSay(request.getSession()->credentials, channel, text);
			}
		}
	}

	return true; // Return empty document
}

ServerWebAppletHandler::AppletAsyncHandler::AppletAsyncHandler() :
	lastMessage_(0),
	initialized_(false)
{
}

bool ServerWebAppletHandler::AppletAsyncHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Check if we have sent the initial data
	if (!initialized_)
	{
		initialized_ = true;

		// Add all of the available chat channels
		{
			std::list<std::string> channels =
				ScorchedServer::instance()->getServerChannelManager().getAllChannels();
			std::list<std::string>::iterator itor;
			for (itor = channels.begin();
				itor != channels.end();
				++itor)
			{
				text.append("<addchannel>").
					append(itor->c_str()).
					append("</addchannel>\n");
			}
		}
	}

	// Add all of the chat message that this applet has not seen
	std::string chatText;
	std::list<ServerChannelManager::MessageEntry> &textsList = 
		ScorchedServer::instance()->getServerChannelManager().getLastMessages();
	if (!textsList.empty())
	{
		std::list<ServerChannelManager::MessageEntry>::reverse_iterator textsListItor;
		for (textsListItor = textsList.rbegin();
			textsListItor != textsList.rend();
			++textsListItor)
		{
			ServerChannelManager::MessageEntry &entry = *textsListItor;
			if (lastMessage_ >= entry.messageid) break;

			std::string cleanText;
			XMLNode::removeSpecialChars(entry.message, cleanText);
			
			chatText.insert(0, 
				S3D::formatStringBuffer("<chat>%s</chat>\n", cleanText.c_str()));
		}
		lastMessage_ = textsList.back().messageid;
		text.append(chatText);
	}

	return true;
}
