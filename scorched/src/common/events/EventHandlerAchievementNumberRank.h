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

#if !defined(__INCLUDE_EventHandlerAchievementNumberRankh_INCLUDE__)
#define __INCLUDE_EventHandlerAchievementNumberRankh_INCLUDE__

#include <events/EventHandlerAchievement.h>

class EventHandlerAchievementNumberRank : public EventHandlerAchievement
{
public:
	EventHandlerAchievementNumberRank(const std::string &name, 
		EventHandlerDataBase *database,
		unsigned int *availableRanks,
		unsigned int numberOfRanks);
	virtual ~EventHandlerAchievementNumberRank();

	virtual unsigned int getCurrentCount(Tank *tank) = 0;
	virtual void incrementCount(Tank *tank);

	virtual void tankConnected(Tank *tank);
	virtual void tankDisconnected(Tank *tank);

protected:
	struct RankInformation
	{
		RankInformation(unsigned int nr = 0, unsigned int num = 0) :
			nextRank(nr),
			currentNumber(num)
		{
		}

		unsigned int nextRank;
		unsigned int currentNumber;
	};

	std::map<unsigned int, RankInformation> playerRanks_;
	std::vector<unsigned int> availableRanks_;

	unsigned int getNextAchievement(unsigned int currentKills);
	unsigned int getCurrentAchievement(unsigned int currentKills);
};

#endif
