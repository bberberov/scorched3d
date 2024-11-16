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

#include <coms/ComsMessageHandler.h>
#include <common/Defines.h>
#include <common/Logger.h>

ComsMessageConnectionHandlerI::~ComsMessageConnectionHandlerI()
{
}

ComsMessageHandlerI::~ComsMessageHandlerI()
{
}

ComsMessageHandler::ComsMessageHandler(const char *instanceName) : 
	instanceName_(instanceName),
	connectionHandler_(0), comsMessageLogging_(false)
{
}

ComsMessageHandler::~ComsMessageHandler()
{
}

void ComsMessageHandler::setConnectionHandler(
		ComsMessageConnectionHandlerI *handler)
{
	connectionHandler_ = handler;
}

void ComsMessageHandler::addHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler)
{
	unsigned int id = comsMessageType.getId();
	if (recvHandlers_.size() < id + 1) recvHandlers_.resize(id + 1);
	recvHandlers_[id] = handler;
}

void ComsMessageHandler::addSentHandler(ComsMessageType &comsMessageType,
		ComsMessageHandlerI *handler)
{
	unsigned int id = comsMessageType.getId();
	if (sentHandlers_.size() < id + 1) sentHandlers_.resize(id + 1);
	sentHandlers_[id] = handler;
}

void ComsMessageHandler::processMessage(NetMessage &message)
{
	switch(message.getMessageType())
	{
		case NetMessage::BufferMessage:
			if (connectionHandler_) 
				connectionHandler_->messageRecv(message.getDestinationId());
			processReceiveMessage(message);
			break;
		case NetMessage::SentMessage:
			if (connectionHandler_) 
				connectionHandler_->messageSent(message.getDestinationId());
			processSentMessage(message);
			break;
		case NetMessage::DisconnectMessage:
			if (comsMessageLogging_)
			{
				Logger::log(S3D::formatStringBuffer("ComsMessage::Disconnected(%i, %i)",
					message.getDestinationId(), message.getFlags()));
			}

			if (connectionHandler_) 
				connectionHandler_->clientDisconnected(message);
			break;
		case NetMessage::ConnectMessage:
			if (comsMessageLogging_)
			{
				Logger::log(S3D::formatStringBuffer("ComsMessage::Connected(%i)",
					message.getDestinationId()));
			}

			if (connectionHandler_)
				connectionHandler_->clientConnected(message);
			break;
		default:
			if (connectionHandler_)
				connectionHandler_->clientError(message,
					"Failed to recognise message type");
			break;
	}
}

void ComsMessageHandler::processReceiveMessage(NetMessage &message)
{
	processMessage(message, recvHandlers_, "RECV");
}

void ComsMessageHandler::processSentMessage(NetMessage &message)
{
	processMessage(message, sentHandlers_, "SEND");
}

void ComsMessageHandler::processMessage(NetMessage &message,
	std::vector<ComsMessageHandlerI *> &handlers,
	const char *sendRecv)
{
	if (handlers.empty()) return;

	// Get how big the buffer is
	unsigned int bufferUsed = message.getBuffer().getBufferUsed();
	if (bufferUsed < 1)
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message,
				S3D::formatStringBuffer("Failed to get %s message compression state", sendRecv));
	}

	// Check if the buffer is sent compressed
	bool compressed = (message.getBuffer().getBuffer()[bufferUsed - 1] == '1');
	message.getBuffer().setBufferUsed(bufferUsed - 1);
	if (compressed)
	{
		if (!message.getBuffer().uncompressBuffer())
		{
			if (connectionHandler_)
				connectionHandler_->clientError(message,
					S3D::formatStringBuffer("Failed to uncompress %s message", sendRecv));
		}
	}
	NetBufferReader reader(message.getBuffer());

	unsigned char messageTypeId;
	if (!reader.getFromBuffer(messageTypeId))
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message,
				S3D::formatStringBuffer("Failed to decode %s message type", sendRecv));
		return;
	}
	ComsMessageType *comsMessageType = ComsMessageType::getTypeForId(messageTypeId);
	if (!comsMessageType)
	{
		if (connectionHandler_) 
			connectionHandler_->clientError(message, 
				S3D::formatStringBuffer("Failed to find %s message type %u", 
				sendRecv, messageTypeId));
		return;
	}
	const char *messageTypeStr = comsMessageType->getName().c_str();
	
	if (comsMessageLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::process%s(%s, %i, %u%s)",
			instanceName_.c_str(),
			sendRecv,
			messageTypeStr, message.getDestinationId(),
			bufferUsed,
			compressed?", compressed":""));
	}
	if (messageTypeId >= handlers.size())
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message, 
				S3D::formatStringBuffer("Failed to find %s message type handler \"%s\"",
					sendRecv, messageTypeStr));
		return;
	}
	ComsMessageHandlerI *handler = handlers[messageTypeId];
	if (!handler)
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message, 
				S3D::formatStringBuffer("Failed to find %s message type handler \"%s\"",
					sendRecv, messageTypeStr));
		return;
	}	
	if (!handler->processMessage(
		message, messageTypeStr, reader))
	{
		if (connectionHandler_)
			connectionHandler_->clientError(message, 
				S3D::formatStringBuffer("Failed to handle %s message type handler \"%s\"",
					sendRecv, messageTypeStr));
		return;
	}

	if (comsMessageLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::processFinished%s(%s, %i)",
			instanceName_.c_str(),
			sendRecv,
			messageTypeStr, message.getDestinationId()));
	}
}
