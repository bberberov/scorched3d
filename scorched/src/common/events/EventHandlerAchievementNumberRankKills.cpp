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

#include <events/EventHandlerAchievementNumberRankKills.h>
#include <tank/Tank.h>

static const std::string NAME = "NumberKills";

static unsigned int killRanks[] = {
	1,
	10,
	25,
	50,
	100,
	250,
	500,
	1000,
	2500,
	5000,
	10000,
	15000,
	20000,
	25000,
	30000,
	35000,
	40000,
	45000,
	50000
};

EventHandlerAchievementNumberRankKills::EventHandlerAchievementNumberRankKills(EventHandlerDataBase *database) :
	EventHandlerAchievementNumberRank(NAME, database, killRanks, sizeof(killRanks)/sizeof(unsigned int))
{

}

EventHandlerAchievementNumberRankKills::~EventHandlerAchievementNumberRankKills()
{
}

unsigned int EventHandlerAchievementNumberRankKills::getCurrentCount(Tank *tank)
{
	if (!database_) return 0;
	return database_->getKillCount(tank->getUniqueId());
}

void EventHandlerAchievementNumberRankKills::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	incrementCount(firedTank);
}
