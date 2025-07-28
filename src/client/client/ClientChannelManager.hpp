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

#if !defined(__INCLUDE_ClientChannelManagerh_INCLUDE__)
#define __INCLUDE_ClientChannelManagerh_INCLUDE__

#include <coms/ComsMessageHandler.h>
#include <client/ClientChannelManagerI.h>
#include <console/ConsoleRule.h>
#include <set>

class ClientChannelManager 
{
public:
	static ClientChannelManager *instance();

	void showText(const ChannelText &text);
	void sendText(const ChannelText &text);

	bool channelActive(const std::string &channelName);

	std::set<unsigned int> &getMutedPlayers() { return mutedPlayers_; }

	bool registerClient(ClientChannelManagerI *reciever,
		std::list<std::string> &channels);
	bool deregisterClient(ClientChannelManagerI *reciever);
	bool changeRegistration(ClientChannelManagerI *reciever,
		std::list<std::string> &channels);

	void addChannel(const char *lookfor, const char *channel);
	void removeChannel(const char *channel);

protected:
	static ClientChannelManager *instance_;
	static unsigned int nextRecieverId_;

	class ChannelEntry
	{
	public:
		ChannelEntry(ClientChannelManagerI *user);

		ClientChannelManagerI *getUser() { return user_; }
		void setChannels(std::list<ChannelDefinition> &channels);
		std::set<std::string> &getChannels() { return channels_; }

		bool hasChannel(const std::string &channel);

	protected:
		ClientChannelManagerI *user_;
		std::set<std::string> channels_;
	};

	std::set<unsigned int> mutedPlayers_;
	std::map<unsigned int, ChannelEntry *> recievers_;
	unsigned int getChannelEntry(ClientChannelManagerI *reciever);
	void say(std::vector<ConsoleRuleValue> &values);
	bool processChannelMessage(NetMessage &message, NetBufferReader &reader);
	bool processChannelTextMessage(NetMessage &message, NetBufferReader &reader);

private:
	ClientChannelManager();
	virtual ~ClientChannelManager();

};

#endif
