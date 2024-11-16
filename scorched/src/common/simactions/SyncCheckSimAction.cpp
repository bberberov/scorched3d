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

#include <simactions/SyncCheckSimAction.h>
#include <coms/ComsSyncCheckMessage.h>
#include <coms/ComsMessageSender.h>
#include <server/ScorchedServer.h>
#include <server/ServerSyncCheck.h>

REGISTER_CLASS_SOURCE(SyncCheckSimAction);

SyncCheckSimAction::SyncCheckSimAction() :
	syncId_(0)
{
}

SyncCheckSimAction::SyncCheckSimAction(unsigned int syncId) :
	syncId_(syncId)
{
}

SyncCheckSimAction::~SyncCheckSimAction()
{
}

bool SyncCheckSimAction::invokeAction(ScorchedContext &context)
{
	if (context.getServerMode())
	{
		ComsSyncCheckMessage *message = new ComsSyncCheckMessage(syncId_, context);
		ScorchedServer::instance()->getServerSyncCheck().addServerSyncCheck(message);
	}
	else
	{
#ifndef S3D_SERVER
		ComsSyncCheckMessage message(syncId_, context);
		ComsMessageSender::sendToServer(message);
#endif 
	}

	return true;
}

bool SyncCheckSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(syncId_);
	return true;
}

bool SyncCheckSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(syncId_)) return false;
	return true;
}
