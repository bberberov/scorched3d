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

#include <client/ClientChannelManager.h>
#include <client/ScorchedClient.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsChannelMessage.h>
#include <coms/ComsChannelTextMessage.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <common/Logger.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <tanket/TanketShotInfo.h>
#include <sprites/TalkRenderer.h>
#include <engine/ActionController.h>

ClientChannelManager::ChannelEntry::ChannelEntry(ClientChannelManagerI *user) :
	user_(user)
{
}

void ClientChannelManager::ChannelEntry::setChannels(std::list<ChannelDefinition> &channels)
{
	channels_.clear();
	std::list<ChannelDefinition>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		++itor)
	{
		channels_.insert(itor->getChannel());
	}
}

bool ClientChannelManager::ChannelEntry::hasChannel(const std::string &channel)
{
	return (channels_.find(channel) != channels_.end());
}

unsigned int ClientChannelManager::nextRecieverId_ = 0;

ClientChannelManager *ClientChannelManager::instance_ = 0;

ClientChannelManager *ClientChannelManager::instance()
{
	if (!instance_)
	{
		instance_ = new ClientChannelManager;
	}
	return instance_;
}

ClientChannelManager::ClientChannelManager()
{
	new ComsMessageHandlerIAdapter<ClientChannelManager>(
		this, &ClientChannelManager::processChannelMessage,
		ComsChannelMessage::ComsChannelMessageType,
		ScorchedClient::instance()->getComsMessageHandler());
	new ComsMessageHandlerIAdapter<ClientChannelManager>(
		this, &ClientChannelManager::processChannelTextMessage,
		ComsChannelTextMessage::ComsChannelTextMessageType,
		ScorchedClient::instance()->getComsMessageHandler());
	new ConsoleRuleMethodIAdapterEx<ClientChannelManager>(
		this, &ClientChannelManager::say, "Say", 
		ConsoleUtil::formParams(
		ConsoleRuleParam("channel", ConsoleRuleTypeString),
		ConsoleRuleParam("text", ConsoleRuleTypeString)));
}

ClientChannelManager::~ClientChannelManager()
{
}

bool ClientChannelManager::registerClient(ClientChannelManagerI *reciever,
	std::list<std::string> &channels)
{
	if (getChannelEntry(reciever) > 0) return false;

	// Add a new reciever
	ChannelEntry *entry = new ChannelEntry(reciever);
	unsigned int channelId = ++nextRecieverId_;
	recievers_[channelId] = entry;

	// Send the request for a new reciever
	ComsChannelMessage message(ComsChannelMessage::eRegisterRequest, channelId);
	std::list<std::string>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		++itor)
	{
		ChannelDefinition entry(itor->c_str());
		message.getChannels().push_back(entry);
	}
	ComsMessageSender::sendToServer(message);

	return true;
}

bool ClientChannelManager::deregisterClient(ClientChannelManagerI *reciever)
{
	unsigned int channelId = getChannelEntry(reciever);
	if (channelId == 0) return false;

	// Remove the reciever
	delete recievers_[channelId];
	recievers_.erase(channelId);

	// Send the request to remove the reciever
	ComsChannelMessage message(ComsChannelMessage::eDeregisterRequest, channelId);
	ComsMessageSender::sendToServer(message);

	return true;
}

bool ClientChannelManager::changeRegistration(ClientChannelManagerI *reciever,
	std::list<std::string> &channels)
{
	unsigned int channelId = getChannelEntry(reciever);
	if (channelId == 0) return false;

	// Send the request to add a new channel
	ComsChannelMessage message(ComsChannelMessage::eJoinRequest, channelId);	
	std::list<std::string>::iterator itor;
	for (itor = channels.begin();
		itor != channels.end();
		++itor)
	{
		ChannelDefinition entry(itor->c_str());
		message.getChannels().push_back(entry);
	}
	ComsMessageSender::sendToServer(message);

	return true;
}

void ClientChannelManager::addChannel(const char *lookfor, const char *channel)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		++itor)
	{
		ChannelEntry *r = (*itor).second;
		if (r->hasChannel(lookfor))
		{
			std::set<std::string> currentChannels = r->getChannels();
			currentChannels.insert(channel);
			std::list<std::string> channels;
			std::set<std::string>::iterator itor;
			for (itor = currentChannels.begin();
				itor != currentChannels.end();
				++itor)
			{
				channels.push_back(*itor);
			}

			changeRegistration(r->getUser(), channels);
			break;
		}
	}
}

void ClientChannelManager::removeChannel(const char *channel)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		++itor)
	{
		ChannelEntry *r = (*itor).second;
		if (r->hasChannel(channel))
		{
			std::set<std::string> currentChannels = r->getChannels();
			currentChannels.erase(channel);
			std::list<std::string> channels;
			std::set<std::string>::iterator itor;
			for (itor = currentChannels.begin();
				itor != currentChannels.end();
				++itor)
			{
				channels.push_back(*itor);
			}

			changeRegistration(r->getUser(), channels);
			break;
		}
	}
}

unsigned int ClientChannelManager::getChannelEntry(ClientChannelManagerI *reciever)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		++itor)
	{
		ChannelEntry *r = (*itor).second;
		if (r->getUser() == reciever) return (*itor).first;
	}
	return 0;
}

void ClientChannelManager::say(std::vector<ConsoleRuleValue> &values)
{
	ConsoleRuleValue &channelValue = values[1];
	ConsoleRuleValue &textValue = values[2];

	ChannelText message(channelValue.valueString.c_str(), 
		LANG_STRING(textValue.valueString));
	sendText(message);
}

void ClientChannelManager::sendText(const ChannelText &constText)
{
	ChannelText text = constText;

	unsigned int playerId = 
		ScorchedClient::instance()->getTargetContainer().getCurrentPlayerId();
	if (!playerId)
	{
		std::map<unsigned int, Tank *> &tanks = 
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = (*itor).second;
			if (tank->getDestinationId() == 
				ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
			{
				playerId = tank->getPlayerId();
				break;
			}
		}
	}

	text.setSrcPlayerId(playerId);
	ComsChannelTextMessage message(text);
	ComsMessageSender::sendToServer(message);
}

void ClientChannelManager::showText(const ChannelText &constText)
{
	ChannelText text = constText;

	// Add this line to the console
	if (!(text.getFlags() & ChannelText::eNoLog))
	{
		std::string mes(LangStringUtil::convertFromLang(text.getMessage()));

		Logger::log(S3D::formatStringBuffer("[%s] : %s",
			text.getChannel().c_str(),
			mes.c_str()));
	}

	// Send to all recievers
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		++itor)
	{
		ChannelEntry *entry = (*itor).second;
		if (entry->hasChannel(text.getChannel()))
		{
			entry->getUser()->channelText(text);
		}
	}
}

bool ClientChannelManager::channelActive(const std::string &channelName)
{
	std::map<unsigned int, ChannelEntry *>::iterator itor;
	for (itor = recievers_.begin();
		itor != recievers_.end();
		++itor)
	{
		ChannelEntry *entry = (*itor).second;
		if (entry->hasChannel(channelName))
		{
			return true;
		}
	}
	return false;
}

bool ClientChannelManager::processChannelMessage(NetMessage &message, 
	NetBufferReader &reader)
{
		// We have a ChannelMessage from the server
		ComsChannelMessage channelMessage;
		if (!channelMessage.readMessage(reader)) return false;

		// Get the reciever for this message
		std::map<unsigned int, ChannelEntry *>::iterator findItor =
			recievers_.find(channelMessage.getId());
		if (findItor == recievers_.end()) return true; // No reciever
		ChannelEntry *entry = (*findItor).second;
		ClientChannelManagerI *reciever = entry->getUser();

		// Check which ChannelMessage was sent
		if (channelMessage.getType() == ComsChannelMessage::eJoinRequest)
		{
			// Its a channels request
			entry->setChannels(channelMessage.getChannels());
			reciever->registeredForChannels(channelMessage.getChannels(),
				channelMessage.getAvailableChannels());
		}
	return true;
}

bool ClientChannelManager::processChannelTextMessage(NetMessage &message, 
	NetBufferReader &reader)
{
		// We have a ChannelTextMessage from the server
		ComsChannelTextMessage textMessage;
		if (!textMessage.readMessage(reader)) return false;

		Tank *tank = ScorchedClient::instance()->getTargetContainer().getTankById(
			textMessage.getChannelText().getSrcPlayerId());
		if(tank && tank->getState().getState() == TankState::sNormal)
		{
			// put a speach bubble over the talking tank
			Vector white(1.0f, 1.0f, 1.0f);
			TalkRenderer *talk = new TalkRenderer(
				tank->getLife().getTankTurretPosition().asVector(),
				white);
			ScorchedClient::instance()->getActionController().
				addAction(new SpriteAction(talk));
		}
		// Ignore any messages from this tank
		if (tank && tank->getState().getMuted()) return true;

		// Log this message
		std::string mes(LangStringUtil::convertFromLang(
			textMessage.getChannelText().getMessage()));
		if (tank)
		{
			Logger::log(S3D::formatStringBuffer("[%s][%s] : %s",
				textMessage.getChannelText().getChannel().c_str(),
				tank->getCStrName().c_str(),
				mes.c_str()));
		}
		else
		{
			Logger::log(S3D::formatStringBuffer("[%s] : %s",
				textMessage.getChannelText().getChannel().c_str(),
				mes.c_str()));
		}

		// Foreach reciever
		std::list<unsigned int>::iterator itor;
		for (itor = textMessage.getIds().begin();
			itor != textMessage.getIds().end();
			++itor)
		{
			unsigned int id = (*itor);

			// Get the reciever for this message
			std::map<unsigned int, ChannelEntry *>::iterator findItor =
				recievers_.find(id);
			if (findItor != recievers_.end())
			{
				ClientChannelManagerI *reciever = (*findItor).second->getUser();
				reciever->channelText(textMessage.getChannelText());
			}
		}
	return true;
}
