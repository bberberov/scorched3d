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

#if !defined(__INCLUDE_GLWChannelViewh_INCLUDE__)
#define __INCLUDE_GLWChannelViewh_INCLUDE__

#include <GLW/GLWChatView.h>
#include <GLW/GLWChannelViewTextRenderer.h>
#include <client/ClientChannelManagerI.h>

class GLWChannelViewI
{
public:
	virtual ~GLWChannelViewI();

	virtual void channelsChanged(unsigned int id) = 0;
};

class GLWChannelView : 
	public GLWChatView,
	public ClientChannelManagerI
{
public:
	class BaseChannelEntry
	{
	public:
		std::string channel;
		unsigned int type;
	};
	class CurrentChannelEntry : public BaseChannelEntry
	{
	public:
		Vector color;
		unsigned int id;
	};

	GLWChannelView();
	virtual ~GLWChannelView();

	virtual bool initFromXMLInternal(XMLNode *node);

	unsigned int getLastWhisperSrc() { return lastWhisperSrc_; }

	std::list<CurrentChannelEntry> &getCurrentChannels() { return currentChannels_; }
	std::list<BaseChannelEntry> &getAvailableChannels() { return availableChannels_; }
	CurrentChannelEntry *getChannel(const std::string &channelName);

	void joinChannel(const std::string &channelName);
	void leaveChannel(const std::string &channelName);

	void setHandler(GLWChannelViewI *handler) { handler_ = handler; };

	// GLWidget
	virtual void draw();

	// ClientChannelManagerI
	virtual void channelText(ChannelText &text);
	virtual void registeredForChannels(
		std::list<ChannelDefinition> &registeredChannels,
		std::list<ChannelDefinition> &availableChannels);

	REGISTER_CLASS_HEADER(GLWChannelView);

protected:
	GLWChannelViewI *handler_;

	std::string textSound_;
	unsigned int lastChannelId_;
	unsigned int lastWhisperSrc_;
	bool showChannelName_, showChannelNumber_;
	std::map<std::string, Vector> channelColors_;
	std::list<std::string> startupChannels_;
	std::list<CurrentChannelEntry> currentChannels_;
	std::list<BaseChannelEntry> availableChannels_;

	void formCurrentChannelList(std::list<std::string> &result);
};

#endif // __INCLUDE_GLWChannelViewh_INCLUDE__
