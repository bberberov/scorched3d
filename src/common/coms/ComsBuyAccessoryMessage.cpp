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

#include <coms/ComsBuyAccessoryMessage.h>

ComsMessageType ComsBuyAccessoryMessage::ComsBuyAccessoryMessageType("ComsBuyAccessoryMessageType");

ComsBuyAccessoryMessage::ComsBuyAccessoryMessage() :
	ComsMessage(ComsBuyAccessoryMessageType),
	playerId_(0), accessoryId_(0),
	buy_(false)	
{
}

ComsBuyAccessoryMessage::ComsBuyAccessoryMessage(ComsBuyAccessoryMessage &other) :
	ComsMessage(ComsBuyAccessoryMessageType),
	playerId_(other.playerId_), accessoryId_(other.accessoryId_),
	buy_(other.buy_)	
{
}

ComsBuyAccessoryMessage::ComsBuyAccessoryMessage(
	unsigned int playerId,
	unsigned int accessoryId,
	bool buy) :
	ComsMessage(ComsBuyAccessoryMessageType),
	playerId_(playerId), accessoryId_(accessoryId),
	buy_(buy)
{
}

ComsBuyAccessoryMessage::~ComsBuyAccessoryMessage()
{
}

bool ComsBuyAccessoryMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(accessoryId_);
	buffer.addToBuffer(buy_);
	return true;
}

bool ComsBuyAccessoryMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(accessoryId_)) return false;
	if (!reader.getFromBuffer(buy_)) return false;
	return true;
}
