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

#if !defined(__INCLUDE_ComsMessageHandlerh_INCLUDE__)
#define __INCLUDE_ComsMessageHandlerh_INCLUDE__

#include <net/NetMessageHandler.h>
#include <coms/ComsMessage.h>
#include <vector>

class ComsMessageConnectionHandlerI
{
public:
	virtual ~ComsMessageConnectionHandlerI();

	virtual void clientConnected(NetMessage &message) = 0;
	virtual void clientDisconnected(NetMessage &message) = 0;
	virtual void clientError(NetMessage &message,
		const std::string &errorString) = 0;

	virtual void messageRecv(unsigned int destinationId) = 0;
	virtual void messageSent(unsigned int destinationId) = 0;
};

class ComsMessageHandlerI
{
public:
	virtual ~ComsMessageHandlerI();

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader) = 0;
};

class ComsMessageHandler : public NetMessageHandlerI
{
public:
	ComsMessageHandler(const char *instanceName = "");
	virtual ~ComsMessageHandler();

	// Used to add a handler for a specific message type
	void setConnectionHandler(
		ComsMessageConnectionHandlerI *handler);
	void addHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler);
	void addSentHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler);

	// Inherited from NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

	bool &getMessageLogging() { return comsMessageLogging_; }

protected:
	std::string instanceName_;
	std::vector<ComsMessageHandlerI *> recvHandlers_;
	std::vector<ComsMessageHandlerI *> sentHandlers_;
	ComsMessageConnectionHandlerI *connectionHandler_;

	bool comsMessageLogging_;
	void processReceiveMessage(NetMessage &message);
	void processSentMessage(NetMessage &message);
	void processMessage(NetMessage &message,
		std::vector<ComsMessageHandlerI *> &handlers,
		const char *sendRecv);
};

// Same as above but passed userdata to method
template<class T>
class ComsMessageHandlerIAdapter : public ComsMessageHandlerI
{
public:
	ComsMessageHandlerIAdapter(T *inst, 
		bool (T::*call)(NetMessage &message, NetBufferReader &reader), 
		ComsMessageType &comsMessageType,
		ComsMessageHandler &handler) :
		inst_(inst), call_(call)
	{
		handler.addHandler(comsMessageType, this);
	};
	virtual ~ComsMessageHandlerIAdapter()
	{
	};

	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader)
	{
		return (inst_->*call_)(message, reader);
	}

protected:
	T *inst_;
	bool (T::*call_)(NetMessage &message, NetBufferReader &reader);
};

#endif
