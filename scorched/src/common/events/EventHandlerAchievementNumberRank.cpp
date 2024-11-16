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

#include <events/EventHandlerAchievementNumberRank.h>
#include <tank/Tank.h>

EventHandlerAchievementNumberRank::EventHandlerAchievementNumberRank(
	const std::string &name, 
	EventHandlerDataBase *database,
	unsigned int *availableRanks,
	unsigned int numberOfRanks) :
	EventHandlerAchievement(name, database)
{
	for (unsigned int i=0; i<numberOfRanks; i++)
	{
		availableRanks_.push_back(availableRanks[i]);
	}
}

EventHandlerAchievementNumberRank::~EventHandlerAchievementNumberRank()
{
}

void EventHandlerAchievementNumberRank::tankConnected(Tank *tank)
{
	unsigned int currentCount = getCurrentCount(tank);
	unsigned int currentRank = getRank(tank->getPlayerId());
	unsigned int actualCurrentRank = 0;

	unsigned int rank = 1;
	std::vector<unsigned int>::iterator itor;
	for (itor = availableRanks_.begin();
		itor != availableRanks_.end();
		++itor, ++rank)
	{
		if (currentCount >= *itor)
		{
			actualCurrentRank = rank;
		} else break;
	}

	if (actualCurrentRank > currentRank) 
	{
		// Give this user an achievement
		awardAchievement(tank->getPlayerId(), actualCurrentRank);
	}

	RankInformation info(actualCurrentRank + 1, currentCount);
	playerRanks_[tank->getPlayerId()] = info;
}

void EventHandlerAchievementNumberRank::tankDisconnected(Tank *tank)
{
	playerRanks_.erase(tank->getPlayerId());
}

void EventHandlerAchievementNumberRank::incrementCount(Tank *tank)
{
	RankInformation &info = playerRanks_[tank->getPlayerId()];
	unsigned int currentCount = ++info.currentNumber;
	unsigned int neededCount = availableRanks_[info.nextRank-1];

	if (currentCount >= neededCount)
	{
		// Assign achivement
		awardAchievement(tank->getPlayerId(), info.nextRank);

		// Update total needed for next achievement
		info.nextRank++;
	}
}
