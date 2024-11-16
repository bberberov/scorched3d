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

#include <server/ServerTurnsFree.h>
#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <common/OptionsScorched.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <tank/Tank.h>
#include <list>
#include <set>

ServerTurnsFree::ServerTurnsFree() :
	ServerTurns(false),
	nextMoveId_(0)
{
}

ServerTurnsFree::~ServerTurnsFree()
{
}


void ServerTurnsFree::internalEnterState()
{
	nextMoveId_++;

	waitingPlayers_.clear();

	std::map<unsigned int, Tanket*> &tankets = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket*>::iterator itor;
	for (itor = tankets.begin();
		itor != tankets.end();
		++itor)
	{
		Tanket *tanket = itor->second;
		tanket->getShotInfo().setMoveId(0);
		if (tanket->getAlive() && !tanket->getTankAI())
		{
			waitingPlayers_.push_back(tanket->getPlayerId());
		}
	}	
}

void ServerTurnsFree::internalSimulate(fixed frameTime)
{
	// Build list of currently playing destinations
	std::set<unsigned int> playingDestinations;
	std::map<unsigned int, Tanket*> &tankets = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket*>::iterator itor;
	for (itor = tankets.begin();
		itor != tankets.end();
		++itor)
	{
		Tanket *tanket = itor->second;
		if (!tanket->getShotInfo().getUseNormalMoves()) continue;

		if (tanket->getShotInfo().getMoveId() != 0)
		{
			if (tanket->getAlive())
			{
				unsigned int destinationId = 0;
				if (tanket->getType() == Target::TypeTank)
				{
					destinationId = ((Tank *) tanket)->getDestinationId();
				}

				if (destinationId != 0)
				{
					playingDestinations.insert(destinationId);
				} 
			}
			else
			{
				playMoveFinished(tanket);
			}
		}
		else if (tanket->getTankAI())
		{
			// Check player isn't delayed
			if (timedPlayers_.find(tanket->getPlayerId()) ==
				timedPlayers_.end())
			{
				playMove(tanket, ++nextMoveId_, fixed(0));
			}
		}
	}

	// Check to see if we can play
	std::list<unsigned int>::iterator waitingItor;
	for (waitingItor = waitingPlayers_.begin();
		waitingItor != waitingPlayers_.end();
		++waitingItor)
	{
		unsigned int playerId = *waitingItor;

		Tanket *tanket = ScorchedServer::instance()->getTargetContainer().getTanketById(playerId);
		if (tanket && 
			tanket->getAlive() &&
			tanket->getShotInfo().getMoveId() == 0)
		{
			unsigned int destinationId = 0;
			if (tanket->getType() == Target::TypeTank)
			{
				destinationId = ((Tank *) tanket)->getDestinationId();
			}

			if (destinationId != 0 &&
				playingDestinations.find(destinationId) == 
				playingDestinations.end() &&
				timedPlayers_.find(tanket->getPlayerId()) ==
				timedPlayers_.end())
			{
				playingDestinations.insert(destinationId);

				playMove(tanket, ++nextMoveId_, fixed(0));

				waitingPlayers_.erase(waitingItor);
				waitingPlayers_.push_back(playerId);
				break;
			}
		}
	}
}

void ServerTurnsFree::internalMoveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	Tanket *tanket = ScorchedServer::instance()->getTargetContainer().getTanketById(playerId);
	if (!tanket || tanket->getShotInfo().getMoveId() != moveId) return;
	
	playMoveFinished(tanket);

	if (tanket->getAlive())
	{
		// Make the move
		std::list<ComsPlayedMoveMessage*> messages;
		messages.push_back(new ComsPlayedMoveMessage(playedMessage));
		playShots(messages, nextMoveId_, false, true);

		// Check to see if this is timed mode
		int delayShotTime = 0;
		if (ScorchedServer::instance()->getOptionsGame().getTurnType() ==
			OptionsGame::TurnFreeTimed)
		{
			delayShotTime = ScorchedServer::instance()->getOptionsGame().getShotTime();
		}
		// Add some thinking time on the AIs shots
		if (tanket->getTankAI())
		{
			int aiShotTime = ScorchedServer::instance()->getOptionsGame().getAIShotTime();
			aiShotTime += (rand() % 5) - 3;
			if (aiShotTime < 0) aiShotTime = 0;
			delayShotTime += aiShotTime;
		}
		if (delayShotTime > 0)
		{
			timedPlayers_[tanket->getPlayerId()] = fixed(delayShotTime);
		}
	}
}
