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

#if !defined(__INCLUDE_ServerWebServerh_INCLUDE__)
#define __INCLUDE_ServerWebServerh_INCLUDE__

#include <net/NetServerTCP.h>
#include <common/FileLogger.h>
#include <webserver/ServerWebServerQueue.h>

class ServerWebServer : public NetMessageHandlerI
{
public:
	static ServerWebServer *instance();

	void start(int port);
	void processMessages();
	void addRequestHandler(const char *url,
		ServerWebServerI *handler);
	void addThrededRequestHandler(const char *url,
		ServerWebServerI *handler);
	void addAsyncRequestHandler(const char *url,
		ServerWebServerI *handler);

protected:
	static ServerWebServer *instance_;

	struct HandlerEntry
	{
		enum Flags
		{
			eAsync = 1,
			eThreaded = 2
		};

		ServerWebServerI *handler;
		unsigned int flags;
	};
	std::map<std::string, HandlerEntry> handlers_;

	ServerWebServerQueue asyncQueue_;
	ServerWebServerQueue threadedQueue_;
	ServerWebServerQueue normalQueue_;

	unsigned int asyncTimer_;
	std::list<std::pair<unsigned int, NetMessage *> > delayedMessages_;

	SDL_Thread *sendThread_;
	NetServerTCP netServer_;
	FileLogger *logger_;

	bool processRequest(
		unsigned int destinationId,
		const char *ip,
		const char *url,
		std::map<std::string, std::string> &fields,
		std::map<std::string, NetMessage *> &parts);
	bool validateUser(
		const char *ip,
		const char *url,
		std::map<std::string, std::string> &fields,
		bool &delayed);
	unsigned int validateSession(
		const char *ip,
		const char *url,
		std::map<std::string, std::string> &fields);

	static int sendThreadFunc(void *);
	bool processQueue(ServerWebServerQueue &queue, bool keepEntries);

	// Inherited from NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

private:
	ServerWebServer();
	virtual ~ServerWebServer();
};

#endif
