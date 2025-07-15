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

#include <client/ClientConnectionAuthHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientState.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <server/ScorchedServer.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/AuthDialog.h>
#include <GLW/GLWWindowManager.h>
#include <coms/ComsConnectAuthMessage.h>
#include <coms/ComsMessageSender.h>
#include <graph/OptionsDisplay.h>
#include <target/TargetContainer.h>
#include <client/SecureID.h>
#include <net/NetInterface.h>

ClientConnectionAuthHandler *ClientConnectionAuthHandler::instance_ = 0;

ClientConnectionAuthHandler *ClientConnectionAuthHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientConnectionAuthHandler();
	}

	return instance_;
}

ClientConnectionAuthHandler::ClientConnectionAuthHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsConnectAuthMessage::ComsConnectAuthMessageType,
		this);
}

ClientConnectionAuthHandler::~ClientConnectionAuthHandler()
{

}

bool ClientConnectionAuthHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsConnectAuthMessage message;
	if (!message.readMessage(reader)) return false;

	bool nameRequired = (0 == strcmp(message.getUserName(), "required"));
	bool passwordRequired = (0 == strcmp(message.getPassword(), "required"));
	if ((passwordRequired && !ClientParams::instance()->getPassword()[0]) ||
		(nameRequired && !ClientParams::instance()->getUserName()[0]))
	{
		AuthDialog::instance()->setRequiredAuth(
			(nameRequired?AuthDialog::eNameRequired:0) |
			(passwordRequired?AuthDialog::ePasswordRequired:0));
		GLWWindowManager::instance()->showWindow(
			AuthDialog::instance()->getId());
	}
	else
	{
		sendAuth();
	}

	return true;
}

void ClientConnectionAuthHandler::sendAuth()
{
	Logger::log("Authenticating");
	ProgressDialog::instance()->progressChange(LANG_RESOURCE("AUTHENTICATING", "Authenticating"), 100);

	const char *hostName = ConnectDialog::instance()->getHost();
	int portNumber = ConnectDialog::instance()->getPort();

	// Update unique id store
	std::string uniqueId, SUI;
	if (ClientParams::instance()->getConnectedToServer())
	{
		IPaddress address;
		if (SDLNet_ResolveHost(&address, (char *) hostName, 0) == 0)
		{
			unsigned int ipAddress = SDLNet_Read32(&address.host);
			uniqueId = ConnectDialog::instance()->getIdStore().getUniqueId(ipAddress);

			SecureID MakeKey;
			SUI = MakeKey.getSecureID(ipAddress);
		}
	}

	// Check the number of players that are connecting
	unsigned int noPlayers = 1;
	if (!ClientParams::instance()->getConnectedToServer() &&
		!ClientParams::instance()->getSaveFile()[0])
	{
		int maxComputerAIs = ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers();
		noPlayers = 0;
		for (int i=0; i<maxComputerAIs; i++)
		{
			const char *playerType = ScorchedServer::instance()->getOptionsGame().getPlayerType(i);
			if (0 == stricmp(playerType, "Human"))
			{
				noPlayers++;
			}
		}
	}

	ComsConnectAuthMessage connectMessage;
	connectMessage.setUserName(ClientParams::instance()->getUserName());
	connectMessage.setPassword(ClientParams::instance()->getPassword());
	connectMessage.setUniqueId(uniqueId.c_str());
	connectMessage.setSUI(SUI.c_str());
	connectMessage.setHostDesc(OptionsDisplay::instance()->getHostDescription());
	connectMessage.setNoPlayers(noPlayers);
	connectMessage.setCompatabilityVer((unsigned int) OptionsDisplay::instance()->getOptions().size());

	if (!ComsMessageSender::sendToServer(connectMessage))
	{
		LangString msg = LANG_RESOURCE_2("FAILED_TO_SEND_AUTH",
			"Failed to send auth to server \"{0}:{1}\", send failed.",
			hostName, portNumber);
		MsgBoxDialog::show(msg);

		cancelAuth();
	}
}

void ClientConnectionAuthHandler::cancelAuth()
{
	ScorchedClient::instance()->getNetInterface().stop();
	ScorchedClient::instance()->getGameState().stimulate(
		ClientState::StimOptions);
}

