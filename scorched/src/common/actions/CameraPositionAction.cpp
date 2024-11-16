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

#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <actions/CameraPositionAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>

CameraPositionAction::CameraPositionAction(
	unsigned int playerId,
	TankViewPointProvider *provider,
	fixed showTime,
	unsigned int priority,
	bool explosion) : 
	Action(false),
	playerId_(playerId),
	totalTime_(0), showTime_(showTime),
	provider_(provider), showPriority_(priority),
	explosion_(explosion)
{
	provider_->incrementReference();
}

CameraPositionAction::~CameraPositionAction()
{
	if (!context_->getServerMode())
	{
		if (playerId_ != 0)
		{
			CameraPositionActionRegistry::rmCameraPositionAction(this);
		}
		Tank *tank = context_->getTargetContainer().getTankById(playerId_);
		if (tank) 
		{
			// Remove projectile
			tank->getViewPoints().getProjectileViewPoints().removeViewPoint(provider_);

			// Remove explosion
			if (explosion_)
			{
				tank->getViewPoints().getExplosionViewPoints().removeViewPoint(provider_);
			}
		}

		// Tidy any tank references
		if (!tank || !tank->getViewPoints().getProjectileViewPoints().hasViewPoints()) 
		{
			TankViewPointsCollection::TankViewPointsTanks.erase(playerId_);
		}
	}
	provider_->decrementReference();
}

void CameraPositionAction::init()
{
	if (!context_->getServerMode())
	{
		if (playerId_ != 0)
		{
			CameraPositionActionRegistry::addCameraPositionAction(this);
		}
		Tank *tank = context_->getTargetContainer().getTankById(playerId_);
		if (tank) 
		{
			// Add projectile
			if (!tank->getViewPoints().getProjectileViewPoints().hasViewPoints())
			{
				TankViewPointsCollection::TankViewPointsTanks.insert(tank->getPlayerId());
			}
			tank->getViewPoints().getProjectileViewPoints().addViewPoint(provider_);

			// Add explosion
			if (explosion_)
			{
				tank->getViewPoints().getExplosionViewPoints().addViewPoint(provider_);
			}
		}
	}
}

void CameraPositionAction::simulate(fixed frameTime, bool &remove)
{
	if (provider_->getReferenceCount() == 1)
	{
		totalTime_ += frameTime;
		if (totalTime_ > showTime_)
		{
			remove = true;
		}
	}

	Action::simulate(frameTime, remove);
}

std::set<CameraPositionAction *> CameraPositionActionRegistry::actions_;
CameraPositionAction *CameraPositionActionRegistry::currentAction_ = 0;

void CameraPositionActionRegistry::addCameraPositionAction(CameraPositionAction *action)
{
	actions_.insert(action);
}

void CameraPositionActionRegistry::rmCameraPositionAction(CameraPositionAction *action)
{
	if (currentAction_ == action) currentAction_ = 0;
	actions_.erase(action);
}

CameraPositionAction *CameraPositionActionRegistry::getCurrentAction()
{
	// Check if the current action is still active
	if (actions_.find(currentAction_) == actions_.end())
	{
		// If the current action is not still going get another one
		currentAction_ = getCurrentBest();
	}
	else 
	{
		// If it is check that it is the best action
		CameraPositionAction *bestAction = getCurrentBest();
		if (bestAction)
		{
			if (bestAction->getShowPriority() > currentAction_->getShowPriority())
			{
				currentAction_ = bestAction;
			}
		}
	}

	return currentAction_;
}

CameraPositionAction *CameraPositionActionRegistry::getCurrentBest()
{
	CameraPositionAction *currentBest = 0;
	if (!actions_.empty())
	{
		std::set<CameraPositionAction *>::iterator itor;
		for (itor = actions_.begin();
			itor != actions_.end();
			++itor)
		{
			CameraPositionAction *action = (*itor);
			
			// Check that this action is near the beginning
			fixed currentTime = action->getScorchedContext()->
				getSimulator().getCurrentTime();
			fixed actionTime = action->getActionStartTime();
			if (currentTime - actionTime < 1)
			{
				// Is there an action to beat
				if (!currentBest) currentBest = action;
				else
				{
					// Yes, so check if it beats it
					if (currentBest->getShowPriority() < action->getShowPriority())
					{
						// There is a new current best
						currentBest = action;
					}
				}
			}
		}
	}
	return currentBest;
}

