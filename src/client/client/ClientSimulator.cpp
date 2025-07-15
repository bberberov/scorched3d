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

#include <client/ClientSimulator.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/ShotCountDown.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsSimulateResultMessage.h>
#include <coms/ComsNetStatMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/Logger.h>

ClientSimulator::ClientSimulator() : 
	GameStateI("ClientSimulator"),
	serverTimeDifference_(10, 0),
	serverChoke_(25, 0),
	loadingLevel_(false)
{
}

ClientSimulator::~ClientSimulator()
{
}

bool ClientSimulator::continueToSimulate()
{
	bool choked = (currentTime_ >= waitingEventTime_);
	serverChoke_.addValue(choked?100:0);
	return !choked;
}

void ClientSimulator::simulate(const unsigned state, float simTime)
{
	if (loadingLevel_) return;
	Simulator::simulate();
}

void ClientSimulator::actualSimulate(fixed frameTime)
{
	Simulator::actualSimulate(frameTime);
	ShotCountDown::instance()->simulateTime(frameTime);
}

bool ClientSimulator::processNetStatMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsNetStatMessage message;
	if (!message.readMessage(reader)) return false;

	// Store some stats
	serverStepTime_ = message.getSendStepSize();
	serverRoundTripTime_ = message.getRoundTripTime();

	return true;
}

bool ClientSimulator::processComsSimulateMessage(
	NetMessage &netMessage,
	NetBufferReader &reader)
{
	ComsSimulateMessage message;
	if (!message.readMessage(reader)) return false;

	if (!loadingLevel_)
	{
		// Actualy process message
		addComsSimulateMessage(message);

		// Send back a response so ping times can be calculated
		ComsSimulateResultMessage resultMessage(message.getServerTime());
		ComsMessageSender::sendToServer(resultMessage);

		// Calculate the difference between the local time and the server time
		fixed difference = (message.getActualTime() + serverRoundTripTime_ / 2) - actualTime_;
		serverTimeDifference_.addValue(difference);

		// Make adjustment to local time
		actualTime_ += serverTimeDifference_.getAverage() / 20;
	}
	else
	{
		// Actualy process message
		addComsSimulateMessage(message);

		// Set the current time to the time in the message
		setSimulationTime(waitingEventTime_);
	}

	return true;
}

void ClientSimulator::addComsSimulateMessage(ComsSimulateMessage &message)
{
	// Set new waiting time
	waitingEventTime_ = message.getEventTime();

	// Add the new actions for this time
	std::list<SimAction *>::iterator itor;
	for (itor = message.getActions().begin();
		itor != message.getActions().end();
		++itor)
	{
		SimAction *action = *itor;
		if (action->replayAction() || !loadingLevel_)
		{
			simActions_.push_back(new SimActionContainer(action, waitingEventTime_));
		}
	}
}

void ClientSimulator::newLevel()
{
	// Remove any remaining actions
	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.front();
		delete container;
		simActions_.pop_front();
	}

	Simulator::newLevel();

	// Reset times
	lastTickTime_ = SDL_GetTicks() - 1;
	serverTimeDifference_.reset(0);
}

void ClientSimulator::setSimulationTime(fixed actualTime)
{
	// Set actual time
	lastTickTime_ = SDL_GetTicks();
	actualTime_ = actualTime;
	serverTimeDifference_.reset(0);

	// Simulate
	Simulator::simulateTime();
}

ClientSimulator::ActionControllerGameState::ActionControllerGameState() :
	GameStateI("ActionControllerGameState")
{
}

void ClientSimulator::ActionControllerGameState::draw(const unsigned int state)
{
	ScorchedClient::instance()->getClientSimulator().getActionController().draw();
}
