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

#include <client/ClientConnectionAcceptHandler.h>
#include <client/ClientLoadLevelHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/ProgressDialog.h>
#include <engine/ModFiles.h>
#include <image/ImageFactory.h>
#include <image/ImagePngFactory.h>
#include <target/TargetContainer.h>
#include <net/NetInterface.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsHaveModFilesMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <common/Defines.h>

ClientConnectionAcceptHandler *ClientConnectionAcceptHandler::instance_ = 0;

ClientConnectionAcceptHandler *ClientConnectionAcceptHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientConnectionAcceptHandler();
	}

	return instance_;
}

ClientConnectionAcceptHandler::ClientConnectionAcceptHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsConnectAcceptMessage::ComsConnectAcceptMessageType,
		this);
}

ClientConnectionAcceptHandler::~ClientConnectionAcceptHandler()
{

}

bool ClientConnectionAcceptHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsConnectAcceptMessage message;
	if (!message.readMessage(reader)) return false;

	if (ClientParams::instance()->getConnectedToServer())
	{
		unsigned int ip = netMessage.getIpAddress();
		if (!ConnectDialog::instance()->getIdStore().saveUniqueId(
			ip, message.getUniqueId(), message.getPublishAddress()))
		{
			Logger::log( "Server failed ip security check!");
			return false;
		}
	}

	// The server tells us what our id is.
	// Set this id so we know what our players are
	ScorchedClient::instance()->getTargetContainer().
		setCurrentDestinationId(message.getDestinationId());

	// Tell the user to wait
	Logger::log(S3D::formatStringBuffer(
		"Connection accepted by \"%s\".\nPlease wait...",
		message.getServerName()));

	// Set the mod
	S3D::setDataFileMod(
		ScorchedClient::instance()->getOptionsGame().getMod());

	// Load any mod files we currently have for the mod
	// the server is using.
	if (ClientParams::instance()->getConnectedToServer())
	{
		if (!ScorchedClient::instance()->getModFiles().loadModFiles(
			ScorchedClient::instance()->getOptionsGame().getMod(), true,
			ProgressDialogSync::events_instance()))
		{
			S3D::dialogMessage("ModFiles", 
				S3D::formatStringBuffer("Failed to load mod \"%s\"",
				ScorchedClient::instance()->getOptionsGame().getMod()));
			return false;
		}
	}

	// Tell the server what mod files we actually have
	ComsHaveModFilesMessage comsFileMessage;
	std::map<std::string, ModFileEntry *> &files = 
		ScorchedClient::instance()->getModFiles().getFiles();
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = files.begin();
		itor != files.end();
		++itor)
	{
		const std::string &name = (*itor).first;
		ModFileEntry *file = (*itor).second;

		comsFileMessage.getFiles().push_back(
			ModIdentifierEntry(
				true,
				name.c_str(),
				file->getUncompressedSize(),
				file->getUncompressedCrc()));
	}
	if (!ComsMessageSender::sendToServer(comsFileMessage)) return false;

	ClientLoadLevelHandler::instance()->setInitialLevel(true);

	// Move into the files state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimLoadFiles);

	return true;
}
