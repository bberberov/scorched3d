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

#include <simactions/TankStartMoveSimAction.h>
#include <server/ScorchedServer.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <tankai/TankAI.h>
#include <engine/ActionController.h>
#include <common/OptionsScorched.h>
#ifndef S3D_SERVER
#include <graph/ShotCountDown.h>
#include <client/ScorchedClient.h>
#include <client/ClientStartGameHandler.h>
#endif

REGISTER_CLASS_SOURCE(TankStartMoveSimAction);

TankStartMoveSimAction::TankStartMoveSimAction()
{
}

TankStartMoveSimAction::TankStartMoveSimAction(
	unsigned int playerId, unsigned int moveId,
	fixed timeout, bool buying, fixed ping) :
	playerId_(playerId), moveId_(moveId),
	timeout_(timeout), buying_(buying), ping_(ping)
{
}

TankStartMoveSimAction::~TankStartMoveSimAction()
{
}

bool TankStartMoveSimAction::invokeAction(ScorchedContext &context)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(playerId_);
	if (!tanket) return true;

	if (tanket->getType() == Target::TypeTank) 
	{
		Tank *tank = (Tank *) tanket;
		tank->getScore().setPing((ping_ * 1000).asInt());

		if (tank->getState().getState() != TankState::sNormal && !buying_) return true;
		if (tank->getState().getState() != TankState::sBuying && buying_) return true;
	}
	else
	{
		if (!tanket->getAlive()) return true;
	}

	if (!context.getServerMode())
	{
		tanket->getShotInfo().setMoveId(moveId_);

#ifndef S3D_SERVER
		if (tanket->getType() == Target::TypeTank) 
		{
			Tank *tank = (Tank *) tanket;
			if (tank->getDestinationId() == context.getTargetContainer().getCurrentDestinationId())
			{
				ClientStartGameHandler::instance()->startGame(this);
				ShotCountDown::instance()->showMoveTime(
					timeout_, 
					buying_?ShotCountDown::eBuying:ShotCountDown::ePlaying,
					playerId_);
			}
		}
#endif
	}
	else
	{
		if (tanket->getTankAI())
		{
			if (buying_)
			{
				tanket->getTankAI()->buyAccessories(moveId_);
			}
			else
			{
				tanket->getTankAI()->playMove(moveId_);
			}
		}
	}

	return true;
}

bool TankStartMoveSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(moveId_);
	buffer.addToBuffer(buying_);
	buffer.addToBuffer(timeout_);
	buffer.addToBuffer(ping_);
	return true;
}

bool TankStartMoveSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(moveId_)) return false;
	if (!reader.getFromBuffer(buying_)) return false;
	if (!reader.getFromBuffer(timeout_)) return false;
	if (!reader.getFromBuffer(ping_)) return false;
	return true;
}