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

#include <server/ServerStateStartingMatch.h>
#include <server/ScorchedServer.h>
#include <server/ServerChannelManager.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankState.h>

ServerStateStartingMatch::ServerStateStartingMatch()
{
}

ServerStateStartingMatch::~ServerStateStartingMatch()
{
}

void ServerStateStartingMatch::reset()
{
	totalTime_ = 0;
}

bool ServerStateStartingMatch::startingMatch(fixed frameTime)
{
	fixed lastTime = totalTime_;
	totalTime_ += frameTime;

	fixed allowedTime(
		ScorchedServer::instance()->getOptionsGame().getStartTime());
	fixed timeleft = allowedTime - totalTime_ + fixed(true, 500);

	if (lastTime.asInt() != totalTime_.asInt())
	{
		if ((timeleft.asInt() % 5 == 0 || timeleft.asInt() <= 5) && (timeleft.asInt() != 0))
		{
			ScorchedServer::instance()->getServerChannelManager().sendText(
				ChannelText("info", 
					"GAME_STARTING_IN_X", 
					"Game starting in {0} seconds...", 
					timeleft.asInt()),
				true);
		}
	}
	if (timeleft <= 0)
	{
		startMatch();
		return true;
	}
	return false;
}

void ServerStateStartingMatch::startMatch()
{
	ScorchedServer::instance()->getContext().getTankTeamScore().newMatch();
	ScorchedServer::instance()->getTankDeadContainer().clearTanks();
	ScorchedServer::instance()->getOptionsTransient().reset();

	ScorchedServer::instance()->getServerChannelManager().sendText(
		ChannelText("info", 
			"GAME_STARTED", 
			"Game started"),
		true);
}

void ServerStateStartingMatch::stoppingMatch()
{
	ScorchedServer::instance()->getServerChannelManager().sendText(
		ChannelText("info", 
			"TOO_FEW_PLAYERS", 
			"Too few players, stopping play"),
		true);
}
