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

#include <simactions/TankAchievementSimAction.h>

REGISTER_CLASS_SOURCE(TankAchievementSimAction);

TankAchievementSimAction::TankAchievementSimAction()
{
}

TankAchievementSimAction::TankAchievementSimAction(unsigned int playerId, 
	const std::string &achievementName, unsigned int rank) :
	playerId_(playerId), achievementName_(achievementName), rank_(rank)
{
}

TankAchievementSimAction::~TankAchievementSimAction()
{
}

bool TankAchievementSimAction::invokeAction(ScorchedContext &context)
{

	return true;
}

bool TankAchievementSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(achievementName_);
	buffer.addToBuffer(rank_);
	return true;
}

bool TankAchievementSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(achievementName_)) return false;
	if (!reader.getFromBuffer(rank_)) return false;
	return true;
}
