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

#if !defined(__INCLUDE_ChannelTexth_INCLUDE__)
#define __INCLUDE_ChannelTexth_INCLUDE__

#include <string>
#include <vector>
#include <net/NetBuffer.h>

class ChannelDefinition
{
public:
	enum ChannelDefinitionTypes
	{
		eReadOnlyChannel = 1,
		eWhisperChannel = 2
	};

	ChannelDefinition(const char *channel = "", 
		unsigned int type = 0);

	const char *getChannel() { return channel_.c_str(); }
	void setChannel(const char *c) { channel_ = c; }

	unsigned int getType() { return type_; }
	void setType(unsigned int t) { type_ = t; }

protected:
	std::string channel_;
	unsigned int type_;
};

class ChannelText
{
public:
	enum Flags
	{
		eNoLog = 1,
		eNoSound = 2
	};

	ChannelText();
	ChannelText(
		const std::string &channel, 
		const LangString &message);
	ChannelText(
		const std::string &channel, 
		const std::string &key,
		const std::string &value);
	ChannelText(
		const std::string &channel, 
		const std::string &key,
		const std::string &value,
		const LangStringConverter &param1);
	ChannelText(
		const std::string &channel, 
		const std::string &key,
		const std::string &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2);
	ChannelText(
		const std::string &channel, 
		const std::string &key,
		const std::string &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2,
		const LangStringConverter &param3);
	ChannelText(
		const std::string &channel, 
		const std::string &key,
		const std::string &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2,
		const LangStringConverter &param3,
		const LangStringConverter &param4);

	void setChannel(const std::string &channel) { channel_ = channel; }
	void setMessage(const LangString &message) { message_ = message; }
	void setMessageKey(const std::string &key) { messageKey_ = key; }
	void setMessageValue(const std::string &value) { messageValue_ = value; }
	void setSrcPlayerId(unsigned int srcPlayerId) { srcPlayerId_ = srcPlayerId; }
	void setSrcPlayerName(const LangString &srcPlayerName) { srcPlayerName_ = srcPlayerName; }
	void setDestPlayerId(unsigned int destPlayerId) { destPlayerId_ = destPlayerId; }
	void setFlags(unsigned int flags) { flags_ = flags; }
	void setAdminPlayer(const std::string &admin) { admin_ = admin; }

	const std::string &getChannel() { return channel_; }
	const LangString &getMessage();
	const std::string &getAdminPlayer() { return admin_; }
	const std::string &getMessageKey() { return messageKey_; }
	const std::string &getMessageValue() { return messageValue_; }
	const std::vector<LangString> &getMessageParams() { return messageParams_; }
	const LangString &getSrcPlayerName() { return srcPlayerName_; }
    unsigned int getSrcPlayerId() { return srcPlayerId_; }
	unsigned int getDestPlayerId() { return destPlayerId_; }
	unsigned int getFlags() { return flags_; }

    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int srcPlayerId_;
	unsigned int destPlayerId_;
	unsigned int flags_;
	std::string messageKey_;
	std::vector<LangString> messageParams_;
	std::string messageValue_;
	std::string channel_;
	LangString message_;
	LangString srcPlayerName_;
	std::string admin_;
};

#endif // __INCLUDE_ChannelTexth_INCLUDE__
