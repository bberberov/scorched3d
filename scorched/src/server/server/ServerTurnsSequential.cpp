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

#include <server/ServerTurnsSequential.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <tank/TankSort.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <list>

ServerTurnsSequential::ServerTurnsSequential() :
	ServerTurns(true),
	playingPlayer_(0), nextMoveId_(0)
{
}

ServerTurnsSequential::~ServerTurnsSequential()
{
}

void ServerTurnsSequential::internalEnterState()
{
	playingPlayer_ = 0;
	waitingPlayers_.clear();

	OptionsGame::TurnType turnType = (OptionsGame::TurnType)
		ScorchedServer::instance()->getOptionsGame().getTurnType().getValue();

	// On the very first round make the order random (if loser first chosen)
	// as there is no loser yet!
	if ((ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() == 1) &&
		turnType == OptionsGame::TurnSequentialLoserFirst)
	{
		turnType = OptionsGame::TurnSequentialRandom;
	}

	// Standard player ordering is the reverse of the tank score
	TankSort::getSortedTanksIds(
		ScorchedServer::instance()->getContext(), 
		waitingPlayers_); // All tanks
	waitingPlayers_.reverse();
	{
		// Tankets wont be on the list, so add them
		std::map<unsigned int, Tanket*> &tankets = 
			ScorchedServer::instance()->getTargetContainer().getTankets();
		std::map<unsigned int, Tanket*>::iterator itor;
		for (itor = tankets.begin();
			itor != tankets.end();
			++itor)
		{
			Tanket *tanket = itor->second;
			if (!tanket->getShotInfo().getUseNormalMoves()) continue;

			tanket->getShotInfo().setMoveId(0);
			if (tanket->getType() == Target::TypeTanket)
			{
				waitingPlayers_.push_back(tanket->getPlayerId());
			}
		}
	}

	// Check for a different ordering
	if (turnType == OptionsGame::TurnSequentialRandom)
	{
		// Create zero player vector
		std::vector<unsigned int> tmpPlayers;
		for (int i=0; i<(int) waitingPlayers_.size(); i++)
		{
			tmpPlayers.push_back(0);
		}

		// Randomize list order into vector
		while (!waitingPlayers_.empty())
		{
			unsigned int player = waitingPlayers_.front();
			waitingPlayers_.pop_front();
			bool done = false;
			while (!done)
			{
				int pos = int(RAND * float(tmpPlayers.size()));
				if (pos < int(tmpPlayers.size()) && tmpPlayers[pos] == 0) 
				{
					tmpPlayers[pos] = player;
					done = true;
				}
			}
		}

		// Copy vector back to list
		for (int i=0; i<(int) tmpPlayers.size(); i++)
		{
			waitingPlayers_.push_back(tmpPlayers[i]);
		}
	}
}

void ServerTurnsSequential::internalSimulate(fixed frameTime)
{
	// Check if all the tanks have made their moves
	if (waitingPlayers_.empty() && playingPlayer_ == 0) 
	{
		internalEnterState();
		incrementTurn();
	}

	// Check tanks are alive
	Tanket *playingTanket =
		ScorchedServer::instance()->getTargetContainer().getTanketById(playingPlayer_);
	if (!playingTanket || !playingTanket->getAlive())
	{
		if (playingTanket && playingTanket->getShotInfo().getMoveId() != 0)
		{
			playMoveFinished(playingTanket);
		}

		playingPlayer_ = 0;
	}

	// Check if we are ready for the next player
	if (playingPlayer_ == 0)
	{
		while (!waitingPlayers_.empty())
		{
			unsigned int waitingPlayer = waitingPlayers_.front();
			waitingPlayers_.pop_front();

			Tanket *waitingTanket =
				ScorchedServer::instance()->getTargetContainer().getTanketById(waitingPlayer);
			if (waitingTanket && waitingTanket->getAlive())
			{
				playingPlayer_ = waitingPlayer;
				makeMove(waitingTanket);
				break;
			}
		}
	}
}

void ServerTurnsSequential::makeMove(Tanket *tanket)
{
	nextMoveId_++;
	fixed shotTime = fixed(
		ScorchedServer::instance()->getOptionsGame().getShotTime());
	playMove(tanket, nextMoveId_, shotTime);
}

void ServerTurnsSequential::internalMoveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	if (playerId != playingPlayer_) return;
	Tanket *tanket = ScorchedServer::instance()->getTargetContainer().getTanketById(playerId);
	if (!tanket || tanket->getShotInfo().getMoveId() != moveId) return;

	playMoveFinished(tanket);
	playingPlayer_ = 0;

	if (tanket->getAlive())
	{
		std::list<ComsPlayedMoveMessage*> messages;
		messages.push_back(new ComsPlayedMoveMessage(playedMessage));
		playShots(messages, moveId, true, true);
	}
}
