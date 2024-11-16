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

#include <client/ClientMessageHandler.h>
#include <client/ClientState.h>
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <target/TargetContainer.h>
#include <lang/LangResource.h>
#include <common/Logger.h>
#include <net/NetInterface.h>

ClientMessageHandler *ClientMessageHandler::instance_ = 0;

ClientMessageHandler *ClientMessageHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientMessageHandler;
	}
	return instance_;
}

ClientMessageHandler::ClientMessageHandler()
{
}

ClientMessageHandler::~ClientMessageHandler()
{
}

void ClientMessageHandler::messageRecv(unsigned int destinationId)
{
}

void ClientMessageHandler::messageSent(unsigned int destinationId)
{
}

void ClientMessageHandler::clientConnected(NetMessage &message)
{
	ConnectDialog::instance()->connected();
}

void ClientMessageHandler::clientDisconnected(NetMessage &message)
{
	if (ClientParams::instance()->getDisconnectTime() == 0)
	{
		ClientParams::instance()->reset();
	}

	Logger::log(S3D::formatStringBuffer("Disconnected"));
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimDisconnected);
	ScorchedClient::instance()->getTargetContainer().setCurrentDestinationId(0);
}

void ClientMessageHandler::clientError(NetMessage &message,
	const std::string &errorString)
{
	Logger::log(S3D::formatStringBuffer("***Client Error*** \"%s\"", errorString.c_str()));
	ScorchedClient::instance()->getNetInterface().disconnectAllClients();
}
