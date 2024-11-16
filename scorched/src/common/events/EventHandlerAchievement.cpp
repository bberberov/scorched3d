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

#include <events/EventHandlerAchievement.h>
#include <events/EventHandlerAchievementNumberRankKills.h>
#include <common/Logger.h>
#include <common/DefinesString.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <simactions/TankAchievementSimAction.h>

EventHandlerAchievement::EventHandlerAchievement(const std::string &name, EventHandlerDataBase *database) :
	name_(name),
	database_(database),
	achievementId_(0)
{
	if (database_)
	{
		achievementId_ = database_->getAchievementId(name);
	} 
	else
	{
		achievementId_ = 1;
	}
}

EventHandlerAchievement::~EventHandlerAchievement()
{
}

unsigned int EventHandlerAchievement::getRank(unsigned int playerId)
{
	if (!database_) return 0;
	return database_->getAchievementRank(achievementId_, playerId);
}

void EventHandlerAchievement::awardAchievement(unsigned int playerId, unsigned int rank)
{
	if (database_) database_->assignAchievementRank(playerId, achievementId_, rank);

	TankAchievementSimAction *action = new TankAchievementSimAction(
		playerId, name_, rank);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);
}
