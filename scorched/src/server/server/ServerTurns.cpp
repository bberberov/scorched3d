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

#include <server/ServerTurns.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerChannelManager.h>
#include <server/ServerDestinations.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankTeamScore.h>
#include <tanket/TanketShotInfo.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/NextTurnSimAction.h>
#include <coms/ComsPlayedMoveMessage.h>

ServerTurns::ServerTurns(bool waitForShots) : 
	shotsState_(eShotsNone),
	waitForShots_(waitForShots),
	nextNonNormalMoveId_(0)
{
	shotsStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::shotsStarted);
	moveStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::moveStarted);
}

ServerTurns::~ServerTurns()
{
	delete shotsStarted_;
	delete moveStarted_;
	{
		std::map<unsigned int, PlayingPlayer*>::iterator itor;
		for (itor = playingPlayers_.begin();
			itor != playingPlayers_.end();
			++itor)
		{
			delete itor->second;
		}
		playingPlayers_.clear();
	}
}

void ServerTurns::enterState()
{
	shotsState_ = eShotsNone;
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		++playingItor)
	{
		delete playingItor->second;
	}
	playingPlayers_.clear();
	timedPlayers_.clear();

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

	internalEnterState();
}

void ServerTurns::internalShotsFinished()
{
}

void ServerTurns::simulate(fixed frameTime)
{
	// Check what we are allowed to do
	if (shotsState_ == eShotsWaitingStart) return;
	if (shotsState_ == eShotsWaitingEnd)
	{
		if (ScorchedServer::instance()->getActionController().noReferencedActions())
		{
			internalShotsFinished();
			shotsState_ = eShotsNone;
		}
		else
		{
			return;
		}
	}

	std::list<unsigned int> processPlayers;
	std::list<unsigned int>::iterator processPlayersItor;

	// Remove time from playing players
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		++playingItor)
	{
		unsigned int playerId = playingItor->first;
		PlayingPlayer *playing = playingItor->second;
		if (playing->startedMove_)
		{
			playing->moveTime_ -= frameTime;
			if (playing->moveTime_ < 0) processPlayers.push_back(playerId);
		}
	}
	for (processPlayersItor = processPlayers.begin();
		processPlayersItor != processPlayers.end();
		++processPlayersItor)
	{
		unsigned int playerId = *processPlayersItor;
		PlayingPlayer *playing = playingPlayers_[playerId];

		// Player has timed out
		ComsPlayedMoveMessage timedOutMessage(
			playerId, playing->moveId_, ComsPlayedMoveMessage::eTimeout);
		moveFinished(timedOutMessage);
	}

	// Fire tankets that are not using the normal move mechanics
	std::map<unsigned int, Tanket*> &tankets = 
		ScorchedServer::instance()->getTargetContainer().getTankets();
	std::map<unsigned int, Tanket*>::iterator itor;
	for (itor = tankets.begin();
		itor != tankets.end();
		++itor)
	{
		Tanket *tanket = itor->second;
		if (tanket->getShotInfo().getMoveId() == 0 &&
			!tanket->getShotInfo().getUseNormalMoves() &&
			tanket->getTankAI())
		{
			// Check player isn't delayed
			if (timedPlayers_.find(tanket->getPlayerId()) ==
				timedPlayers_.end())
			{
				playMove(tanket, ++nextNonNormalMoveId_, fixed(0));
			}
		}
	}

	// Process the delay (if any)
	std::map<unsigned int, fixed>::iterator timedItor;
	for (timedItor = timedPlayers_.begin();
		timedItor != timedPlayers_.end();
		++timedItor)
	{
		timedItor->second -= frameTime;
		if (timedItor->second <= 0)
		{
			timedPlayers_.erase(timedItor);
			break;
		}
	}	

	// Do other stuff
	internalSimulate(frameTime);
}

void ServerTurns::shotsStarted(fixed simulationTime, SimAction *action)
{
	shotsState_ = eShotsWaitingEnd;
}

void ServerTurns::moveStarted(fixed simulationTime, SimAction *action)
{
	TankStartMoveSimAction *startMove = (TankStartMoveSimAction *) action;
	
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(startMove->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		findItor->second->startedMove_ = true;
	}
}

void ServerTurns::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	unsigned int playerId = playedMessage.getPlayerId();
	unsigned int moveId = playedMessage.getMoveId();

	// Check to see if this a tanket that is not using the normal move mechanics
	Tanket *tanket = ScorchedServer::instance()->getTargetContainer().getTanketById(playerId);
	if (tanket && !tanket->getShotInfo().getUseNormalMoves())
	{
		if (tanket->getShotInfo().getMoveId() == moveId)
		{
			playMoveFinished(tanket);
			if (tanket->getAlive() &&
				tanket->getTankAI())
			{			
				// Make the move
				std::list<ComsPlayedMoveMessage*> messages;
				messages.push_back(new ComsPlayedMoveMessage(playedMessage));
				playShots(messages, ++nextNonNormalMoveId_, false, false);

				// Add some thinking time on the AIs shots
				int aiShotTime = ScorchedServer::instance()->getOptionsGame().getAIShotTime();
				aiShotTime += (rand() % 5) - 3;
				if (aiShotTime > 0)
				{
					timedPlayers_[tanket->getPlayerId()] = fixed(aiShotTime);
				}
			}
		}
	}
	else
	{
		internalMoveFinished(playedMessage);
	}
}

bool ServerTurns::finished()
{
	return internalFinished();
}

bool ServerTurns::internalFinished()
{
	return showScore();
}

bool ServerTurns::showScore()
{
	// Check why this round has finished
	int teamWonGame = 
		ScorchedServer::instance()->getContext().getTankTeamScore().getWonGame();
	if (teamWonGame > 0)
	{
		// A team has won
		ChannelText text("info",
			"ROUND_FINISHED_TEAM_WON",
			"Round finished due to team obtaining an objective.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
		ScorchedServer::instance()->getTargetContainer().teamCount() == 1)
	{
		// Only one team left
		ChannelText text("info",
			"ROUND_FINISHED_TEAM",
			"Round finished due to last team standing.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}
	else if (ScorchedServer::instance()->getTargetContainer().aliveCount() == 0)
	{
		// Only one person left
		ChannelText text("info",
			"ROUND_FINISHED_DEAD",
			"Round finished due to annihilation.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}
	else if (ScorchedServer::instance()->getTargetContainer().aliveCount() <= 1)
	{
		// Only one person left
		ChannelText text("info",
			"ROUND_FINISHED_PLAYER",
			"Round finished due to last man standing.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	if (ScorchedServer::instance()->getOptionsGame().getNoTurns() > 0)
	{
		if (ScorchedServer::instance()->getOptionsTransient().getCurrentTurnNo() >
			ScorchedServer::instance()->getOptionsGame().getNoTurns())
		{
			// Max turns exceeded
			ChannelText text("info",
				"ROUND_FINISHED_TURNS",
				"Round finished due to maximum moves reached.");
			ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
			return true;
		}
	}

	// Check for tanks skiping turns
	bool allSkipped = true;
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if (tank->getScore().getWonGame())
		{
			ChannelText text("info",
				"ROUND_FINISHED_TANK_WON",
				"Round finished due to player obtaining an objective.");
			ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
			return true;
		}

		if (tank->getState().getLives() > 0 &&
			tank->getState().getTankPlaying() &&
			tank->getShotInfo().getSkippedShots() < 3)
		{
			allSkipped = false;
		}
	}
	if (allSkipped) 
	{
		ChannelText text("info",
			"ROUND_FINISHED_SKIP",
			"Round finished due to all players skipping moves.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	return false;
}

void ServerTurns::playMove(Tanket *tanket, unsigned int moveId, fixed maximumShotTime)
{
	// Get the ping
	fixed ping = 0;
	if (tanket->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) tanket;
		ServerDestination *destination = 
			ScorchedServer::instance()->getServerDestinations().getDestination(tank->getDestinationId());
		if (destination) ping = destination->getPing().getAverage();
	}

	// Create the move action
	tanket->getShotInfo().setMoveId(moveId);
	TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(
		tanket->getPlayerId(), tanket->getShotInfo().getMoveId(), maximumShotTime, false, ping);

	// If shotTime > 0 then add to the list of playing players so they can be timed out
	SimulatorI *callback = 0;
	if (maximumShotTime > 0)
	{
		fixed simulationTime(ScorchedServer::instance()->getServerSimulator().getSendStepSize());
		PlayingPlayer *playingPlayer = new PlayingPlayer(moveId, maximumShotTime + simulationTime * 2);
		playingPlayers_[tanket->getPlayerId()] = playingPlayer;
		callback = moveStarted_;
	}

	// Delay players that need delayed
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction, callback);
}

void ServerTurns::playMoveFinished(Tanket *tanket)
{	
	// Remove timeout check for this player
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(tanket->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		delete findItor->second;
		playingPlayers_.erase(findItor);
	}

	// Send stop move action
	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(tanket->getPlayerId());
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
	tanket->getShotInfo().setMoveId(0);
}

void ServerTurns::playShots(std::list<ComsPlayedMoveMessage *> messages, unsigned int moveId, bool timeOutPlayers, bool referenced)
{
	PlayMovesSimAction *movesAction = 
		new PlayMovesSimAction(moveId, timeOutPlayers, referenced);
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages.begin();
		itor != messages.end();
		++itor)
	{
		movesAction->addMove(*itor);
	}
	
	SimulatorI *callback = 0;
	if (waitForShots_)
	{
		callback = shotsStarted_;
		shotsState_ = eShotsWaitingStart;
	}
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction, callback);
}

void ServerTurns::incrementTurn()
{
	NextTurnSimAction *turnAction = new NextTurnSimAction();
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(turnAction);
}
