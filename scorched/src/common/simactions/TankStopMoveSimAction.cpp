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

#include <simactions/TankStopMoveSimAction.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/ShotCountDown.h>
#endif

REGISTER_CLASS_SOURCE(TankStopMoveSimAction);

TankStopMoveSimAction::TankStopMoveSimAction()
{
}

TankStopMoveSimAction::TankStopMoveSimAction(unsigned int playerId) :
	playerId_(playerId)
{
}

TankStopMoveSimAction::~TankStopMoveSimAction()
{
}

bool TankStopMoveSimAction::invokeAction(ScorchedContext &context)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(playerId_);
	if (!tanket) return true;

	stopMove(context, tanket);

	return true;
}

void TankStopMoveSimAction::stopMove(ScorchedContext &context, Tanket *tanket)
{
	if (!context.getServerMode())
	{
		tanket->getShotInfo().setMoveId(0);

#ifndef S3D_SERVER
		if (tanket->getType() == Target::TypeTank)
		{
			Tank *tank = (Tank *) tanket;
			if (tank->getDestinationId() == context.getTargetContainer().getCurrentDestinationId())
			{

				ScorchedClient::instance()->getTargetContainer().setCurrentPlayerId(0);
				ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
				ShotCountDown::instance()->hideMoveTime();
			}
		}
#endif
	}
}

bool TankStopMoveSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	return true;
}

bool TankStopMoveSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	return true;
}