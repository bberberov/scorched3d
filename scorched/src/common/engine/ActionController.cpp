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

#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <list>

ActionController::ActionController() : 
	referenceCount_(0),
	context_(0),
	actionProfiling_(false),
	newActions_(10),
	actions_(1000)
{

}

ActionController::~ActionController()
{
	clear(false);
}

void ActionController::clear(bool warn)
{
	// New actions
	for (int a=0; a<newActions_.actionCount; a++)
	{
		Action *act = newActions_.actions[a];
		if (warn)
		{
			Logger::log(S3D::formatStringBuffer("Warning: removing added timed out action %s",
				act->getActionType().c_str()));
		}
		delete act;
	}
	newActions_.clear();

	// Current actions
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		if (warn)
		{
			Logger::log(S3D::formatStringBuffer("Warning: removing added timed out action %s",
				act->getActionType().c_str()));
		}
		delete act;
	}
	actions_.clear();

	// Ref count
	referenceCount_ = 0;
}

void ActionController::logActions()
{
	Logger::log(S3D::formatStringBuffer("ActionLog : New %i, Ref %i",
		newActions_.actionCount,
		referenceCount_));
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		Logger::log(S3D::formatStringBuffer("Action : %s", act->getActionType().c_str()));
	}
}

void ActionController::stopActionProfiling()
{
	if (!actionProfiling_) return;
	actionProfiling_ = false;

	Logger::log("Logging Profiled Actions --------------------");
	int totalCount = 0;
	std::map<std::string, int>::iterator itor;
	for (itor =  actionProfile_.begin();
		itor != actionProfile_.end();
		++itor)
	{
		const std::string &name = (*itor).first;
		int count = (*itor).second;
		totalCount += count;
		Logger::log(S3D::formatStringBuffer("%s - %i", name.c_str(), count));
	}
	Logger::log(S3D::formatStringBuffer("Total - %i", totalCount));
	Logger::log("---------------------------------------------");

	actionProfile_.clear();
}

bool ActionController::noReferencedActions()
{
	bool finished = 
		newActions_.empty() && 
		(referenceCount_ == 0);
	return finished;
}

void ActionController::setScorchedContext(ScorchedContext *context)
{
	context_ = context;
}

void ActionController::addAction(Action *action)
{
	action->setScorchedContext(context_);
	newActions_.push_back(action);
}

void ActionController::addNewActions(fixed time)
{
	if (newActions_.empty()) return;

	int syncActionCount = 0;
	for (int a=0; a<newActions_.actionCount; a++)
	{
		Action *action = newActions_.actions[a];

		// Initialize it
		action->setScorchedContext(context_);
		action->setActionStartTime(time);
		action->init();
		actions_.push_back(action);
		
		// Log it
		if (action->getActionSyncCheck())
		{
			if (context_->getOptionsGame().getActionSyncCheck())
			{
				syncActionCount++;
				std::string actionType = action->getActionType();
				std::string actionDetails = action->getActionDetails();
				context_->getSimulator().addSyncCheck(
					S3D::formatStringBuffer("Added Action : %i %s:%s", 
						syncActionCount,
						actionType.c_str(), 
						actionDetails.c_str()));
			}
		}
		if (actionProfiling_)
		{
			std::map<std::string, int>::iterator findItor =
				actionProfile_.find(action->getActionType());
			if (findItor == actionProfile_.end())
			{
				actionProfile_[action->getActionType()] = 1;
			}
			else
			{
				(*findItor).second++;
			}	
		}

		if (action->getReferenced()) referenceCount_ ++;
	}
	newActions_.clear();
}

void ActionController::draw()
{
	// Itterate and draw all of the actions
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		act->draw();
	}
}

void ActionController::simulate(fixed frameTime, fixed time)
{
	// Ensure any new actions are added
	addNewActions(time);

	// Itterate and draw all of the actions
	int keepcount=0;
	for (int a=0; a<actions_.actionCount; a++)
	{
		Action *act = actions_.actions[a];
		bool remove = false;
		act->simulate(frameTime, remove);

		// Ensure that no referenced actions over do their time
		if (act->getReferenced())
		{
			if ((time - act->getActionStartTime() > 30))
			{
				Logger::log(S3D::formatStringBuffer(
					"Warning: removing timed out action %s",
					act->getActionType().c_str()));
				remove = true;
			}
		}

		// If this action has finished add to list to be removed
		if (remove)
		{
			if (act->getReferenced())
			{
				referenceCount_--;
				if (referenceCount_<0) referenceCount_ = 0;
			}
			if (act->getActionSyncCheck())
			{
				if (context_->getOptionsGame().getActionSyncCheck())
				{
					std::string actionType = act->getActionType();
					std::string actionDetails = act->getActionDetails();
					context_->getSimulator().addSyncCheck(
						S3D::formatStringBuffer("Rm Action : %s:%s", 
							actionType.c_str(), 
							actionDetails.c_str()));
				}
			}

			delete act;
		}
		else
		{
			actions_.actions[keepcount] = act;
			keepcount++;
		}
	}
	actions_.actionCount = keepcount;
}

