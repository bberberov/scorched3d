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

#include <coms/ComsChannelMessage.h>

ComsMessageType ComsChannelMessage::ComsChannelMessageType("ComsChannelMessageType");

ComsChannelMessage::ComsChannelMessage() :
	ComsMessage(ComsChannelMessageType)
{
}

ComsChannelMessage::ComsChannelMessage(RequestType type, unsigned int id) :
	ComsMessage(ComsChannelMessageType),
	type_(type), id_(id)
{
}

ComsChannelMessage::~ComsChannelMessage()
{
}

bool ComsChannelMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) type_);
	buffer.addToBuffer(id_);
	buffer.addToBuffer((int) channels_.size());
	std::list<ChannelDefinition>::iterator itor;
	for (itor = channels_.begin();
		itor != channels_.end();
		++itor)
	{
		buffer.addToBuffer(itor->getType());
		buffer.addToBuffer(itor->getChannel());
	}
	buffer.addToBuffer((int) availableChannels_.size());
	for (itor = availableChannels_.begin();
		itor != availableChannels_.end();
		++itor)
	{
		buffer.addToBuffer(itor->getType());
		buffer.addToBuffer(itor->getChannel());
	}
	return true;
}

bool ComsChannelMessage::readMessage(NetBufferReader &reader)
{
	int type, size;
	if (!reader.getFromBuffer(type)) return false;
	type_ = (RequestType) type;
	if (!reader.getFromBuffer(id_)) return false;
	if (!reader.getFromBuffer(size)) return false;
	for (int s=0; s<size; s++)
	{
		unsigned int channelType;
		std::string channelChannel;
		if (!reader.getFromBuffer(channelType)) return false;
		if (!reader.getFromBuffer(channelChannel)) return false;
		channels_.push_back(
			ChannelDefinition(channelChannel.c_str(), channelType));
	}
	if (!reader.getFromBuffer(size)) return false;
	for (int s=0; s<size; s++)
	{
		unsigned int channelType;
		std::string channelChannel;
		if (!reader.getFromBuffer(channelType)) return false;
		if (!reader.getFromBuffer(channelChannel)) return false;
		availableChannels_.push_back(
			ChannelDefinition(channelChannel.c_str(), channelType));
	}
	return true;
}
