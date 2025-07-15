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

#include <coms/ComsChannelTextMessage.h>

ComsMessageType ComsChannelTextMessage::ComsChannelTextMessageType("ComsChannelTextMessageType");

ComsChannelTextMessage::ComsChannelTextMessage() :
	ComsMessage(ComsChannelTextMessageType)
{
}

ComsChannelTextMessage::ComsChannelTextMessage(ChannelText &text) :
	ComsMessage(ComsChannelTextMessageType),
	channelText_(text)
{
}

ComsChannelTextMessage::~ComsChannelTextMessage()
{
}

bool ComsChannelTextMessage::writeMessage(NetBuffer &buffer)
{
	if (!channelText_.writeMessage(buffer)) return false;
	buffer.addToBuffer((int) ids_.size());
	std::list<unsigned int>::iterator itor;
	for (itor = ids_.begin();
		itor != ids_.end();
		++itor)
	{
		buffer.addToBuffer(*itor);
	}
	return true;
}

bool ComsChannelTextMessage::readMessage(NetBufferReader &reader)
{
	if (!channelText_.readMessage(reader)) return false;
	unsigned int id;
	int size;
	if (!reader.getFromBuffer(size)) return false;
	for (int s=0; s<size; s++)
	{
		if (!reader.getFromBuffer(id)) return false;
		ids_.push_back(id);
	}
	return true;
}
