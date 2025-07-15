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

#include <client/ClientLinesHandler.h>
#include <client/ScorchedClient.h>
#include <coms/ComsLinesMessage.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <GLW/GLWPlanView.h>

ClientLinesHandler *ClientLinesHandler::instance_ = 0;

ClientLinesHandler *ClientLinesHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientLinesHandler;
	}
	return instance_;
}

ClientLinesHandler::ClientLinesHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsLinesMessage::ComsLinesMessageType,
		this);
}

ClientLinesHandler::~ClientLinesHandler()
{
}

bool ClientLinesHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsLinesMessage message;
	if (!message.readMessage(reader)) return false;

	Tank *tank = ScorchedClient::instance()->
		getTargetContainer().getTankById(message.getPlayerId());
	if (!tank || tank->getState().getMuted()) return true;

	std::list<GLWPlanView *>::iterator itor;
	for (itor = callbacks_.begin();
		itor != callbacks_.end();
		++itor)
	{
		GLWPlanView *view = (*itor);
		view->addRecievePoints(
			message.getPlayerId(), message.getLines());
	}

	return true;
}

void ClientLinesHandler::registerCallback(GLWPlanView *planView)
{
	callbacks_.push_back(planView);
}
