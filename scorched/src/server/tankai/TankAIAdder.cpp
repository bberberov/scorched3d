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

#include <tank/Tank.h>
#include <tankai/TankAIAdder.h>
#include <events/EventHandlerDataBase.h>
#include <common/OptionsScorched.h>
#include <target/TargetContainer.h>
#include <server/ServerConnectAuthHandler.h>

unsigned int TankAIAdder::getNextTankId(const char *uniqueId, ScorchedContext &context, std::set<unsigned int> &takenPlayerIds)
{
	// Try to use the persistent stats id
	if (uniqueId[0])
	{
		unsigned int id = 0;
		if (ScorchedServer::instance()->getEventHandlerDataBase())
		{
			id = ScorchedServer::instance()->getEventHandlerDataBase()->getStatsId(uniqueId);
		}
		if (id != 0 &&
			!context.getTargetContainer().getTargetById(id) &&
			takenPlayerIds.find(id) == takenPlayerIds.end())
		{
			DIALOG_ASSERT(id >= TargetID::MIN_TANK_ID && id <= TargetID::MAX_TANK_ID);
			return id;
		}
	}

	// Get the transient id
	static unsigned int id = TargetID::START_TRANSIENT_TANK_ID;
	do 
	{
		++id;
		if (id >= TargetID::MAX_TANK_ID) id = TargetID::START_TRANSIENT_TANK_ID;
	}
	while (context.getTargetContainer().getTargetById(id) ||
		takenPlayerIds.find(id) != takenPlayerIds.end());

	DIALOG_ASSERT(id >= TargetID::START_TRANSIENT_TANK_ID && id <= TargetID::MAX_TANK_ID);
	return id;
}

unsigned int TankAIAdder::getNextTargetId(ScorchedContext &context)
{
	unsigned int targetId_ = TargetID::MIN_TARGET_TRANSIENT_ID;
	while (context.getTargetContainer().getTargetById(targetId_))
	{
		++targetId_;
		if (targetId_ >= TargetID::MAX_TARGET_ID) targetId_ = TargetID::MIN_TARGET_TRANSIENT_ID;
	}

	return targetId_;
}

void TankAIAdder::addTankAIs(ScorchedServer &context)
{
	// On the server
	// Ensure that we cannot add more ais than the server is setup for
	int maxComputerAIs = context.getOptionsGame().getNoMaxPlayers();
	for (int i=0; i<maxComputerAIs; i++)
	{
		const char *playerType = 
			context.getOptionsGame().getPlayerType(i);
		if (0 != stricmp(playerType, "Human"))
		{
			addTankAI(context, playerType);
		}
	}
}

void TankAIAdder::addTankAI(ScorchedServer &context, const char *aiName)
{
	context.getServerConnectAuthHandler().addTankAI(aiName);
}
