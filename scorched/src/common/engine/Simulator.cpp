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

#include <engine/Simulator.h>
#include <engine/ScorchedContext.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <movement/TargetMovement.h>
#include <common/OptionsScorched.h>
#include <landscapemap/LandscapeMaps.h>
#include <SDL/SDL.h>

static const fixed StepSize = fixed(true, fixed::FIXED_RESOLUTION / Sint64(50));

Simulator::Simulator() :
	speed_(1),
	currentTime_(0),
	actualTime_(0),
	context_(0)
{
	lastTickTime_ = SDL_GetTicks();
}

Simulator::~Simulator()
{
	clear();
}

void Simulator::clear()
{
	actionController_.clear();
	events_.clear();
	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.back();
		delete container;
		simActions_.pop_back();
	}
}

void Simulator::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
	actionController_.setScorchedContext(context);
	wind_.setScorchedContext(context);
	random_.setScorchedContext(context);
}

void Simulator::simulate()
{
	unsigned int currentTime = SDL_GetTicks();
	unsigned int timeDiff = currentTime - lastTickTime_;
	lastTickTime_ = currentTime;

	if (timeDiff > 0)
	{
		fixed fixedTimeDiff(true, timeDiff * 10);
		fixedTimeDiff *= speed_;

		actualTime_ += fixedTimeDiff;
		simulateTime();
	}
}

void Simulator::simulateTime()
{
	while (actualTime_ - currentTime_ >= StepSize)
	{
		if (!continueToSimulate()) break;

		// Simulate
		actualSimulate(StepSize);

		// More time has passed
		currentTime_ += StepSize;
	}
}

void Simulator::actualSimulate(fixed frameTime)
{
	// Move the targets
	context_->getTargetMovement().simulate(*context_, frameTime);

	// Move the actions
	actionController_.simulate(frameTime, currentTime_);

	events_.simulate(frameTime, *context_);
	wind_.simulate(frameTime);

	while (!simActions_.empty())
	{
		SimActionContainer *container = simActions_.front();
		if (container->fireTime_ > currentTime_) break;
		simActions_.pop_front();

		if (context_->getOptionsGame().getActionSyncCheck() &&
			container->action_->replayAction())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("Invoking sim action : %s:%s", 
				container->fireTime_.asQuickString(),
				container->action_->getClassName()));
		}

		container->action_->invokeAction(*context_);
		if (container->callback_) 
		{
			container->callback_->actionInvoked(currentTime_, container->action_);
		}
		delete container;
	}
}

void Simulator::addSyncCheck(const std::string &msg)
{
	syncCheck_.push_back(
		S3D::formatStringBuffer(
			">%s %s<", currentTime_.asQuickString(), msg.c_str()));
}

void Simulator::newLevel()
{
	// Clear any action controller actions
	actionController_.clear();
	syncCheck_.clear();
	context_->getTargetMovement().reset();

	// Reset times
	currentTime_ = 0;
	actualTime_ = 0;
	lastTickTime_ = SDL_GetTicks();

	// Reset events
	random_.seed(context_->getLandscapeMaps().getDefinitions().getSeed());
	events_.initialize(*context_);
	wind_.newLevel();

	// Log initial target positions
	if (context_->getOptionsGame().getActionSyncCheck() ||
		context_->getOptionsGame().getTargetPlacementSyncCheck()) 
	{
		addSyncCheck(S3D::formatStringBuffer("New Level"));

		std::map<unsigned int, Tank *> &tanks =
			context_->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = itor->second;
			addSyncCheck(S3D::formatStringBuffer("Tank : %u:%u %s %s - %s", 
				tank->getDestinationId(), tank->getPlayerId(),  
				tank->getCStrName().c_str(), 
				tank->getState().getSmallStateString(),
				tank->getLife().getTargetPosition().asQuickString()));
		}
	}
	if (context_->getOptionsGame().getTargetPlacementSyncCheck())
	{
		std::map<unsigned int, Target*> &targets =
			context_->getTargetContainer().getTargets();
		std::map<unsigned int, Target*>::iterator itor;
		for (itor = targets.begin();
			itor != targets.end();
			++itor)
		{
			Target *target = itor->second;
			addSyncCheck(S3D::formatStringBuffer("Target : %u %s", 
				target->getPlayerId(),  
				target->getLife().getTargetPosition().asQuickString()));
		}
	}
}
