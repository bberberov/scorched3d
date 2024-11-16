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

#include <GLW/GLWChannelView.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWColors.h>
#include <GLEXT/GLState.h>
#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>
#include <XML/XMLParser.h>
#include <common/Keyboard.h>
#include <common/ToolTipResource.h>
#include <client/ClientChannelManager.h>
#include <client/ScorchedClient.h>
#include <sound/SoundUtils.h>
#include <target/TargetContainer.h>
#include <lang/LangResource.h>
#include <lang/LangParam.h>
#include <tank/Tank.h>

GLWChannelViewI::~GLWChannelViewI()
{
}

REGISTER_CLASS_SOURCE(GLWChannelView);

GLWChannelView::GLWChannelView() : lastChannelId_(1), lastWhisperSrc_(0),
	showChannelName_(true), showChannelNumber_(true),
	handler_(0)
{
}

GLWChannelView::~GLWChannelView()
{
	ClientChannelManager::instance()->deregisterClient(this);
}

GLWChannelView::CurrentChannelEntry *GLWChannelView::getChannel(const std::string &channelName)
{
	std::list<CurrentChannelEntry>::iterator itor;
	for (itor = currentChannels_.begin();
		itor != currentChannels_.end();
		++itor)
	{
		CurrentChannelEntry &current = *itor;
		if (channelName == current.channel) return &current;
		if (current.id == atoi(channelName.c_str())) return &current;
	}
	return 0;
}

void GLWChannelView::registeredForChannels(
	std::list<ChannelDefinition> &registeredChannels,
	std::list<ChannelDefinition> &availableChannels)
{
	// Some not efficient stuff....
	std::list<CurrentChannelEntry> oldCurrentChannels = currentChannels_;
	std::list<BaseChannelEntry> oldAvailableChannels = availableChannels_;
	currentChannels_.clear();
	availableChannels_.clear();

	// For all of the new channels
	{
		std::list<ChannelDefinition>::iterator itor;
		for (itor = registeredChannels.begin();
			itor != registeredChannels.end();
			++itor)
		{
			std::string channel = itor->getChannel();

			// Find out if we were already in this channel
			bool found = false;
			std::list<CurrentChannelEntry>::iterator olditor;
			for (olditor = oldCurrentChannels.begin();
				olditor != oldCurrentChannels.end();
				++olditor)
			{
				CurrentChannelEntry &oldEntry = (*olditor);
				if (channel == oldEntry.channel)
				{
					// Add the existing channel, under the existing number
					found = true;
					currentChannels_.push_back(oldEntry);
					break;
				}
			}
			if (!found)
			{
				// Add a new channel with a new number
				CurrentChannelEntry newEntry;
				newEntry.id = lastChannelId_++;
				newEntry.color = Vector(0.8f, 0.8f, 0.8f);
				newEntry.channel = channel;
				newEntry.type = itor->getType();

				std::map<std::string, Vector>::iterator colorItor =
					channelColors_.find(channel.c_str());
				if (colorItor != channelColors_.end()) 
				{
					newEntry.color = colorItor->second;
				}

				currentChannels_.push_back(newEntry);
			}	
		}
	}

	// For all of the available channels
	{
		std::list<ChannelDefinition>::iterator itor;
		for (itor = availableChannels.begin();
			itor != availableChannels.end();
			++itor)
		{
			std::string channel = itor->getChannel();

			// Add this channel to list of available
			BaseChannelEntry newEntry;
			newEntry.channel = channel;
			newEntry.type = itor->getType();
			availableChannels_.push_back(newEntry);
		}
	}

	if (handler_) handler_->channelsChanged(getId());
}

void GLWChannelView::joinChannel(const std::string &channelName)
{
	std::list<std::string> channels;
	formCurrentChannelList(channels);
	channels.push_back(channelName);

	ClientChannelManager::instance()->changeRegistration(this, channels);
}

void GLWChannelView::leaveChannel(const std::string &channelName)
{
	std::list<std::string> channels;
	formCurrentChannelList(channels);
	channels.remove(channelName);

	ClientChannelManager::instance()->changeRegistration(this, channels);
}

void GLWChannelView::channelText(ChannelText &channelText)
{
	if (!OptionsDisplay::instance()->getNoChannelTextSound())
	{
		if (!(channelText.getFlags() & ChannelText::eNoSound) &&
			!textSound_.empty())
		{
			CACHE_SOUND(sound, S3D::getModFile(textSound_.c_str()));
			SoundUtils::playRelativeSound(VirtualSoundPriority::eText, sound);	
		}
	}

	CurrentChannelEntry *channel = getChannel(channelText.getChannel());
	if (!channel) return;

	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(channelText.getSrcPlayerId());
	if (tank)
	{
		if (channel->type & ChannelDefinition::eWhisperChannel)
		{
			if (tank != ScorchedClient::instance()->getTargetContainer().getCurrentTank())
			{
				lastWhisperSrc_ = tank->getPlayerId();
			}
		}
	}

	LangString channelName;
	if (showChannelNumber_)
	{
		channelName.append(LANG_PARAM_1("{0}. ", channel->id));
	}
	if (showChannelName_)
	{
		channelName.append(LANG_PARAM_1("[c:{0}]. ", channel->channel));
	}
	if (tank)
	{
		channelName.append(LANG_PARAM_1("[p:{0}]", tank->getTargetName()));
	}
	else if (channelText.getSrcPlayerName().size() > 0)
	{
		channelName.append(LANG_PARAM_1("[{0}]", channelText.getSrcPlayerName()));
	}
	if (channelText.getAdminPlayer()[0])
	{
		channelName.append(LANG_PARAM_1("[a:{0}]", channelText.getAdminPlayer()));
	}
	if (tank || channelText.getAdminPlayer()[0])
	{
		channelName.append(LANG_STRING(" : "));
	}
	channelName.append(channelText.getMessage());

	GLWChannelViewTextRenderer chanText;
	chanText.parseText(ScorchedClient::instance()->getContext(), channelName);
	
	int currentLen = 0;
	const LangString &text = chanText.getString();
	int totalLen = (int) text.size();
	while (currentLen < totalLen)
	{
		// Get the next split position
		int partLen = splitLine(&text[currentLen]);
		bool nl=(text[currentLen + partLen - 1] == '\n');

		// Create the new text and add it
		GLWChannelViewTextRenderer *newText = new GLWChannelViewTextRenderer();
		newText->subset(chanText, currentLen, (nl?partLen-1:partLen));
		addChat(channel->color, newText->getString(), newText);

		// Increment the current position
		currentLen += partLen;
	}
}

void GLWChannelView::draw()
{
	if (!init_)
	{
		ClientChannelManager::instance()->registerClient(this, startupChannels_);
	}

	GLWChatView::draw();
}

void GLWChannelView::formCurrentChannelList(std::list<std::string> &result)
{
	std::list<CurrentChannelEntry>::iterator itor;
	for (itor = currentChannels_.begin();
		itor != currentChannels_.end();
		++itor)
	{
		CurrentChannelEntry &entry = *itor;
		result.push_back(entry.channel);
	}
}

bool GLWChannelView::initFromXMLInternal(XMLNode *node)
{
	GLWChatView::initFromXMLInternal(node);

	if (!node->getNamedChild("showchannelname", showChannelName_)) return false;
	if (!node->getNamedChild("showchannelnumber", showChannelNumber_)) return false;

	node->getNamedChild("textsound", textSound_, false);

	std::string startupchannel;
	while (node->getNamedChild("channel", startupchannel, false))
	{
		startupChannels_.push_back(startupchannel);
	}

	XMLNode *channelcolors;
	while (node->getNamedChild("channelcolor", channelcolors, false))
	{
		std::string channel;
		Vector color;
		if (!channelcolors->getNamedChild("channel", channel)) return false;
		if (!channelcolors->getNamedChild("color", color)) return false;
		channelColors_[channel] = color;
	}

	return true;
}
