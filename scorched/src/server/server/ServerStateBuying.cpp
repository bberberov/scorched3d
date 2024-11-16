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

#include <server/ServerStateBuying.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerDestinations.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketAccessories.h>
#include <tank/TankDeadContainer.h>
#include <tankai/TankAI.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <simactions/TankBuyingSimAction.h>

ServerStateBuying::ServerStateBuying() :
	nextMoveId_(0)
{
	buyingStarted_ = new SimulatorIAdapter<ServerStateBuying>(this,  &ServerStateBuying::buyingStarted);
}

ServerStateBuying::~ServerStateBuying()
{
	delete buyingStarted_;
	buyingStarted_ = 0;
	std::map<unsigned int, BuyingPlayer*>::iterator itor;
	for (itor = buyingPlayers_.begin();
		itor != buyingPlayers_.end();
		++itor)
	{
		delete itor->second;
	}
	buyingPlayers_.clear();
}

void ServerStateBuying::enterState()
{
	bool firstRound =
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() == 1);
	boughtPlayers_.clear();
	totalTime_ = 0;

	std::map<unsigned int, BuyingPlayer*>::iterator buyingItor;
	for (buyingItor = buyingPlayers_.begin();
		buyingItor != buyingPlayers_.end();
		++buyingItor)
	{
		delete buyingItor->second;
	}
	buyingPlayers_.clear();
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = itor->second;
		tank->getShotInfo().setMoveId(0);
		if (firstRound) tank->getState().setNewlyJoined(true);
	}
}

bool ServerStateBuying::simulate(fixed frameTime)
{
	// Check options
	bool buying =
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
		ScorchedServer::instance()->getOptionsGame().getBuyOnRound());
	if (ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons())
	{
		buying = false;
	}
	fixed buyingTime(ScorchedServer::instance()->getOptionsGame().getBuyingTime());
	fixed simulationTime(ScorchedServer::instance()->getServerSimulator().getSendStepSize());
	bool timeExpired = false;
#ifdef S3D_SERVER
	if (buyingTime != 0)
	{
		totalTime_ += frameTime;
		if (totalTime_ > buyingTime + simulationTime * 2) timeExpired = true;
	}
#endif

	// Add any new players that should be buying
	std::set<unsigned int> playingDestinations;
	bool loading = false;
	bool dead = false;
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = itor->second;
		if (tank->getState().getState() == TankState::sDead)
		{
			dead = true;

			// Check if this a new match for the tank
			if (tank->getState().getNewlyJoined())
			{ 
				boughtPlayers_.erase(tank->getPlayerId());
				tank->getState().setNewlyJoined(false);

				TankNewMatchSimAction *tankNewMatchAction = 
					new TankNewMatchSimAction(tank->getPlayerId());
				if (tank->getUniqueId()[0])
				{
					ScorchedServer::instance()->getTankDeadContainer().getDeadTank(
						tankNewMatchAction, tank->getUniqueId());
				}
				else if (tank->getSUI()[0])
				{
					ScorchedServer::instance()->getTankDeadContainer().getDeadTank(
						tankNewMatchAction, tank->getSUI());
				}
					
				ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankNewMatchAction);
			}

			if (TankBuyingSimAction::getRunningPlayerIds().find(tank->getPlayerId()) ==
				TankBuyingSimAction::getRunningPlayerIds().end())
			{
				boughtPlayers_.erase(tank->getPlayerId());

				TankBuyingSimAction *tankBuyingAction = 
					new TankBuyingSimAction(tank->getPlayerId());
				ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankBuyingAction);
			}
		}
		else if (tank->getState().getState() == TankState::sBuying) 
		{
			// Check if this tank should buy
			if (buying)
			{
				if (tank->getShotInfo().getMoveId() == 0)
				{
					if (boughtPlayers_.find(tank->getPlayerId()) == boughtPlayers_.end() &&
						(tank->getDestinationId() == 0 ||
						playingDestinations.find(tank->getDestinationId()) == playingDestinations.end()))
					{
						playingDestinations.insert(tank->getDestinationId());
						playerBuying(tank->getPlayerId());
					}
				}
				else
				{
					playingDestinations.insert(tank->getDestinationId());
				}

			}
		} 
		else if (tank->getState().getState() == TankState::sLoading) 
		{
			loading = true;
		}
	}

	// Remove time from buying players
	std::list<unsigned int> processPlayers;
	std::list<unsigned int>::iterator processPlayersItor;
	std::map<unsigned int, BuyingPlayer*>::iterator buyingItor;
	for (buyingItor = buyingPlayers_.begin();
		buyingItor != buyingPlayers_.end();
		++buyingItor)
	{
		unsigned int playerId = buyingItor->first;
		BuyingPlayer *buying = buyingItor->second;
		if (buying->startedMove_)
		{
			buying->moveTime_ -= frameTime;
			if (buying->moveTime_ < 0) processPlayers.push_back(playerId);
		}
	}
	for (processPlayersItor = processPlayers.begin();
		processPlayersItor != processPlayers.end();
		++processPlayersItor)
	{
		unsigned int playerId = *processPlayersItor;
		BuyingPlayer *buying = buyingPlayers_[playerId];

		// Player has timed out
		ComsPlayedMoveMessage timedOutMessage(
			playerId, buying->moveId_, ComsPlayedMoveMessage::eTimeout);
		buyingFinished(timedOutMessage);
	}

	// Check if all the tanks have made their moves
	if (!dead)
	{
		if (playingDestinations.empty()) 
		{
			if (!loading || timeExpired)
			{
				return true;
			}
		}
	}

	return false;
}

void ServerStateBuying::playerBuying(unsigned int playerId)
{
	nextMoveId_++;

	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	tank->getShotInfo().setMoveId(nextMoveId_);

	fixed buyingTime
		(ScorchedServer::instance()->getOptionsGame().getBuyingTime());
#ifdef S3D_SERVER
	if (buyingTime != 0)
	{
		buyingTime = buyingTime - totalTime_;
		if (buyingTime < 1) buyingTime = 1;
	}
#endif

	fixed ping = 0;
	ServerDestination *destination = 
		ScorchedServer::instance()->getServerDestinations().getDestination(tank->getDestinationId());
	if (destination) ping = destination->getPing().getAverage();

	TankStartMoveSimAction *tankSimAction = 
		new TankStartMoveSimAction(playerId, nextMoveId_, buyingTime, true, ping);
	SimulatorI *callback = 0;
	if (buyingTime > 0)
	{
		BuyingPlayer *buyingPlayer = new BuyingPlayer(nextMoveId_, buyingTime);
		buyingPlayers_[tank->getPlayerId()] = buyingPlayer;
		callback = buyingStarted_;
	}

	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction, callback);
}

void ServerStateBuying::buyingStarted(fixed simulationTime, SimAction *action)
{
	TankStartMoveSimAction *startMove = (TankStartMoveSimAction *) action;
	
	// Set that this player has started playing
	std::map<unsigned int, BuyingPlayer*>::iterator findItor =
		buyingPlayers_.find(startMove->getPlayerId());
	if (findItor != buyingPlayers_.end())
	{
		findItor->second->startedMove_ = true;
	}
}

void ServerStateBuying::buyingFinished(ComsPlayedMoveMessage &playedMessage)
{
	// Remove timeout check for this player
	std::map<unsigned int, BuyingPlayer*>::iterator findItor =
		buyingPlayers_.find(playedMessage.getPlayerId());
	if (findItor != buyingPlayers_.end())
	{
		delete findItor->second;
		buyingPlayers_.erase(findItor);
	}

	// Check if this player is valid
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();
	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	if (!tank || !tank->getState().getTankPlaying()) return;
	if (moveId != tank->getShotInfo().getMoveId()) return;

	// Set this player to finished buying
	boughtPlayers_.insert(tank->getPlayerId());
	tank->getShotInfo().setMoveId(0);

	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(playerId);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
}
