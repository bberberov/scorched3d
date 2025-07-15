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

#include <server/ServerTurnsSimultaneous.h>
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

ServerTurnsSimultaneous::ServerTurnsSimultaneous(bool waitForShots) :
	ServerTurns(waitForShots),
	nextMoveId_(0)
{
}

ServerTurnsSimultaneous::~ServerTurnsSimultaneous()
{
	std::map<unsigned int, ComsPlayedMoveMessage*>::iterator itor;
	for (itor = moves_.begin();
		itor != moves_.end();
		++itor)
	{
		delete itor->second;
	}
	moves_.clear();
}

void ServerTurnsSimultaneous::internalEnterState()
{
	nextMoveId_++;

	std::map<unsigned int, ComsPlayedMoveMessage*>::iterator movesItor;
	for (movesItor = moves_.begin();
		movesItor != moves_.end();
		++movesItor)
	{
		delete movesItor->second;
	}
	moves_.clear();

	std::map<unsigned int, Tanket*> &tankets = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket*>::iterator itor;
	for (itor = tankets.begin();
		itor != tankets.end();
		++itor)
	{
		Tanket *tanket = itor->second;
		tanket->getShotInfo().setMoveId(0);
	}	
	waitingTime_ = ScorchedServer::instance()->getOptionsGame().getAIShotTime();
}

void ServerTurnsSimultaneous::internalShotsFinished()
{
	incrementTurn();
}

void ServerTurnsSimultaneous::internalSimulate(fixed frameTime)
{
	// Build list of currently playing destinations
	std::set<unsigned int> playingDestinations;
	std::map<unsigned int, Tanket*> &tankets = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket*>::iterator itor;

	bool playersLeft = false;
	bool finished = true;
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

		if (tanket->getAlive())
		{
			if (!tanket->getTankAI())
			{
				playersLeft = true;
			}
			if (moves_.find(tanket->getPlayerId()) == moves_.end())
			{
				finished = false;
			}
		}
	}

	// Check if we need to delay the shots (or they will be very fast)
	if (!waitForShots_ && !playersLeft)
	{
		waitingTime_ -= frameTime;
		if (waitingTime_ > fixed(0)) return;
		waitingTime_ = ScorchedServer::instance()->getOptionsGame().getAIShotTime() / 2;
	}
	
	for (itor = tankets.begin();
		itor != tankets.end();
		++itor)
	{
		Tanket *tanket = itor->second;
		if (tanket->getAlive() &&
			tanket->getShotInfo().getMoveId() == 0)
		{
			if (moves_.find(tanket->getPlayerId()) == moves_.end())
			{
				unsigned int destinationId = 0;
				if (tanket->getType() == Target::TypeTank)
				{
					destinationId = ((Tank *) tanket)->getDestinationId();
				}
				if (playingDestinations.find(destinationId) == 
					playingDestinations.end())
				{
					if (destinationId != 0)
					{
						playingDestinations.insert(destinationId);
					}

					fixed shotTime = fixed(
						ScorchedServer::instance()->getOptionsGame().getShotTime());
					playMove(tanket, ++nextMoveId_, shotTime);
				}
			}
		}
	}

	if (finished)
	{
		std::list<ComsPlayedMoveMessage*> messages;
		std::map<unsigned int, ComsPlayedMoveMessage*>::iterator movesItor;
		for (movesItor = moves_.begin();
			movesItor != moves_.end();
			++movesItor)
		{
			unsigned int playerId = movesItor->first;
			ComsPlayedMoveMessage *message = movesItor->second;

			Tanket *tanket = ScorchedServer::instance()->
				getTargetContainer().getTanketById(playerId);
			if (tanket && tanket->getAlive())
			{
				messages.push_back(message);
			}
			else
			{
				delete message;
			}
		}
		moves_.clear();
		playShots(messages, nextMoveId_, true, true);
		if (!waitForShots_)
		{
			incrementTurn();
		}
	}
}

void ServerTurnsSimultaneous::internalMoveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	Tanket *tanket = ScorchedServer::instance()->getTargetContainer().getTanketById(playerId);
	if (!tanket || tanket->getShotInfo().getMoveId() != moveId) return;
	if (moves_.find(playerId) != moves_.end()) return;
	
	playMoveFinished(tanket);
	
	moves_[playerId] = new ComsPlayedMoveMessage(playedMessage);
}
