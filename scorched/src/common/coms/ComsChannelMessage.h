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

#if !defined(__INCLUDE_ComsChannelMessageh_INCLUDE__)
#define __INCLUDE_ComsChannelMessageh_INCLUDE__

#include <coms/ComsMessage.h>
#include <common/ChannelText.h>
#include <list>

class ComsChannelMessage : public ComsMessage
{
public:
	static ComsMessageType ComsChannelMessageType;

	enum RequestType
	{
		eNoRequest = 0,
		eRegisterRequest = 1,
		eDeregisterRequest = 2,
		eJoinRequest = 3
	};

	ComsChannelMessage();
	ComsChannelMessage(RequestType type, unsigned int id);
	virtual ~ComsChannelMessage();

	RequestType getType() { return type_; }
	unsigned int getId() { return id_; }
	std::list<ChannelDefinition> &getChannels() { return channels_; }
	std::list<ChannelDefinition> &getAvailableChannels() { return availableChannels_;	}

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	RequestType type_;
	unsigned int id_;
	std::list<ChannelDefinition> channels_;
	std::list<ChannelDefinition> availableChannels_;

private:
	ComsChannelMessage(const ComsChannelMessage &);
	const ComsChannelMessage & operator=(const ComsChannelMessage &);

};

#endif
