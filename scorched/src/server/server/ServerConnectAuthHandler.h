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


#if !defined(__INCLUDE_ServerConnectAuthHandlerh_INCLUDE__)
#define __INCLUDE_ServerConnectAuthHandlerh_INCLUDE__

#include <coms/ComsMessageHandler.h>
#include <coms/ComsConnectAuthMessage.h>

class ServerConnectAuthHandler : public ComsMessageHandlerI
{
public:
	ServerConnectAuthHandler(ComsMessageHandler &comsMessageHandler);
	virtual ~ServerConnectAuthHandler();

	void processMessages();

	void addTankAI(std::string tankAi) { aiAdditions_.push_back(tankAi); }
	bool outstandingRequests() { return (!authMessages_.empty() || !aiAdditions_.empty()); }

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader);

protected:
	struct AuthMessage
	{
		unsigned int ipAddress, destinationId;
		ComsConnectAuthMessage message;
	};

	static ServerConnectAuthHandler *instance_;
	std::list<AuthMessage *> authMessages_;
	std::list<std::string> aiAdditions_;

	void processAIInternal(
		std::list<std::string> &aiAdditions);
	void processMessageInternal(
		unsigned int destinationId,
		unsigned int ipAddress,
		ComsConnectAuthMessage &message);
	bool checkAuthSettings(
		unsigned int destinationId,
		unsigned int ipAddress,
		ComsConnectAuthMessage &message);
	void addNextTank(unsigned int destinationId,
		unsigned int ipAddress,
		const char *uniqueId,
		const char *SUI,
		const char *hostDesc,
		bool extraSpectator);

	bool uniqueIdTaken(const std::string &uniqueId);
};


#endif
