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

#include <server/ServerState.h>
#include <server/ServerSyncCheck.h>
#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>

ServerState::ServerState() :
	serverState_(ServerStartupState)
{
}

ServerState::~ServerState()
{
}

void ServerState::simulate(fixed frameTime)
{
	ServerStateEnoughPlayers::Result enoughResult;
	switch (serverState_)
	{
	case ServerStartupState:
		newGame_.newGame();
		serverState_ = ServerWaitingForPlayersState;
		break;
	case ServerWaitingForPlayersState:
		enoughResult = enoughPlayers_.enoughPlayers();
		if (enoughResult == ServerStateEnoughPlayers::eEnough)
		{
			startingMatch_.reset();
			serverState_ = ServerMatchCountDownState;
		}
		else if (enoughResult == ServerStateEnoughPlayers::eNotEnough)
		{
			fixed cycleTime = ScorchedServer::instance()->getOptionsGame().getIdleCycleTime();
			if (ScorchedServer::instance()->getServerSimulator().getCurrentTime() > cycleTime)
			{
				serverState_ = ServerStartupState;
			}
		}
		break;
	case ServerMatchCountDownState:
		enoughResult = enoughPlayers_.enoughPlayers();
		if (enoughResult  == ServerStateEnoughPlayers::eEnough)
		{
			if (startingMatch_.startingMatch(frameTime))
			{
				serverState_ = ServerNewLevelState;
			}
		}
		else if (enoughResult  == ServerStateEnoughPlayers::eNotEnough)
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		break;
	case ServerNewLevelState:
		newGame_.newGame();

		serverState_ = ServerBuyingState;
		buying_.enterState();
		break;
	case ServerBuyingState:
		if (buying_.simulate(frameTime))
		{
			serverState_ = ServerTankNewGameState;
			tankNewGame_.enterState();
		}
		break;
	case ServerTankNewGameState:
		if (tankNewGame_.simulate())
		{
			serverState_ = ServerPlayingState;
			playing_.enterState();
		}
		break;
	case ServerPlayingState:
		if (playing_.showScore() || 
			enoughPlayers_.enoughPlayers() == ServerStateEnoughPlayers::eNotEnough)
		{
			serverState_ = ServerFinishWaitState;
			finishWait_.enterState();
		}
		else 
		{
			playing_.simulate(frameTime);
		}	
		break;
	case ServerFinishWaitState:
		if (finishWait_.simulate())
		{
			serverState_ = ServerScoreState;
			score_.enterState(enoughPlayers_);
		}
		break;
	case ServerScoreState:
		if (score_.simulate())
		{
			if (score_.overAllWinner())
			{
				serverState_ = ServerWaitingForPlayersState;
			}
			else
			{
				serverState_ = ServerNewLevelState;
			}
		}
		break;
	}

	ScorchedServer::instance()->getServerSyncCheck().simulate();
}

void ServerState::buyingFinished(ComsPlayedMoveMessage &message)
{
	if (getState() != ServerState::ServerBuyingState) return;
	buying_.buyingFinished(message);
}

void ServerState::moveFinished(ComsPlayedMoveMessage &message)
{
	if (getState() != ServerState::ServerPlayingState) return;
	playing_.moveFinished(message);
}

void ServerState::scoreFinished()
{
	if (getState() != ServerState::ServerScoreState) return;
	score_.scoreFinished();
}
