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

#include <client/ClientAdminResultHandler.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <coms/ComsAdminResultMessage.h>

ClientAdminResultHandler *ClientAdminResultHandler::instance()
{
	static ClientAdminResultHandler *instance = 
		new ClientAdminResultHandler;
	return instance;
}

ClientAdminResultHandler::ClientAdminResultHandler() : sid_(0)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsAdminResultMessage::ComsAdminResultMessageType,
		this);
}

ClientAdminResultHandler::~ClientAdminResultHandler()
{
}

bool ClientAdminResultHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsAdminResultMessage message;
	if (!message.readMessage(reader)) return false;

	sid_ = message.getSid();

	{
		std::set<ClientAdminResultHandlerI *>::iterator itor;
		for (itor = handlers_.begin();
			itor != handlers_.end();
			++itor)
		{
			(*itor)->adminResult(message.getSid(), message.getType());
		}
	}

	return true;
}
