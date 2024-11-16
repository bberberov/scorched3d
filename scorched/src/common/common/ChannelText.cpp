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

#include <common/ChannelText.h>
#include <lang/Lang.h>

ChannelText::ChannelText() : 
	srcPlayerId_(0), destPlayerId_(0),
	flags_(0)
{
}

ChannelText::ChannelText(
	const std::string &channel, 
	const LangString &message) : 
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	message_(message),
	flags_(0)
{
}

ChannelText::ChannelText(
	const std::string &channel, 
	const std::string &key,
	const std::string &value) :
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	messageValue_(value),
	flags_(0),
	messageKey_(key)
{
}

ChannelText::ChannelText(
	const std::string &channel, 
	const std::string &key,
	const std::string &value,
	const LangStringConverter &param1) :
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	messageValue_(value),
	flags_(0),
	messageKey_(key)
{
	messageParams_.push_back(param1.getValue());
}

ChannelText::ChannelText(
	const std::string &channel, 
	const std::string &key,
	const std::string &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2) :
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	messageValue_(value),
	flags_(0),
	messageKey_(key)
{
	messageParams_.push_back(param1.getValue());
	messageParams_.push_back(param2.getValue());
}

ChannelText::ChannelText(
	const std::string &channel, 
	const std::string &key,
	const std::string &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2,
	const LangStringConverter &param3) :
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	messageValue_(value),
	flags_(0),
	messageKey_(key)
{
	messageParams_.push_back(param1.getValue());
	messageParams_.push_back(param2.getValue());
	messageParams_.push_back(param3.getValue());
}

ChannelText::ChannelText(
	const std::string &channel, 
	const std::string &key,
	const std::string &value,
	const LangStringConverter &param1,
	const LangStringConverter &param2,
	const LangStringConverter &param3,
	const LangStringConverter &param4) :
	srcPlayerId_(0), destPlayerId_(0),
	channel_(channel),
	messageValue_(value),
	flags_(0),
	messageKey_(key)
{
	messageParams_.push_back(param1.getValue());
	messageParams_.push_back(param2.getValue());
	messageParams_.push_back(param3.getValue());
	messageParams_.push_back(param4.getValue());
}

const LangString &ChannelText::getMessage() 
{ 
	if (message_.empty() && !messageKey_.empty())
	{
		ResourceBundleEntry *entry = 
			Lang::instance()->getEntry(messageKey_, messageValue_);
		switch (messageParams_.size())
		{
		case 1:
			message_ = entry->getString(messageParams_[0]);
			break;
		case 2:
			message_ = entry->getString(messageParams_[0], 
				messageParams_[1]);
			break;
		case 3:
			message_ = entry->getString(messageParams_[0], 
				messageParams_[1], messageParams_[2]);
			break;
		case 4:
			message_ = entry->getString(messageParams_[0], 
				messageParams_[1], messageParams_[2], messageParams_[3]);
			break;
		default:
			message_ = entry->getString();
			break;
		}
	}

	return message_; 
}

bool ChannelText::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(channel_);
	buffer.addToBuffer(messageKey_.empty()?message_:LangString());
	buffer.addToBuffer(srcPlayerId_);
	buffer.addToBuffer(destPlayerId_);
	buffer.addToBuffer(admin_);
	buffer.addToBuffer(flags_);
	buffer.addToBuffer(srcPlayerName_);
	buffer.addToBuffer(messageKey_);
	if (!messageKey_.empty())
	{
		buffer.addToBuffer(messageValue_);
		buffer.addToBuffer((int) messageParams_.size());
		std::vector<LangString>::iterator itor;
		for (itor = messageParams_.begin();
			itor != messageParams_.end();
			++itor)
		{
			buffer.addToBuffer(*itor);
		}
	}

	return true;
}

bool ChannelText::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(channel_)) return false;
	if (!reader.getFromBuffer(message_)) return false;
	if (!reader.getFromBuffer(srcPlayerId_)) return false;
	if (!reader.getFromBuffer(destPlayerId_)) return false;
	if (!reader.getFromBuffer(admin_)) return false;
	if (!reader.getFromBuffer(flags_)) return false;
	if (!reader.getFromBuffer(srcPlayerName_)) return false;
	if (!reader.getFromBuffer(messageKey_)) return false;
	if (!messageKey_.empty())
	{
		if (!reader.getFromBuffer(messageValue_)) return false;
		int paramSize = 0;
		if (!reader.getFromBuffer(paramSize)) return false;
		for (int p=0; p<paramSize; p++)
		{
			LangString param;
			if (!reader.getFromBuffer(param)) return false;
			messageParams_.push_back(param);
		}
	}

	return true;
}

ChannelDefinition::ChannelDefinition(const char *c, unsigned int t) :
	channel_(c), type_(t)
{
}
