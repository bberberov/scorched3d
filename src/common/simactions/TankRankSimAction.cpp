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

#include <simactions/TankRankSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <events/EventHandlerDataBase.h>

REGISTER_CLASS_SOURCE(TankRankSimAction);

TankRankSimAction::TankRankSimAction()
{

}

TankRankSimAction::~TankRankSimAction()
{
}

bool TankRankSimAction::invokeAction(ScorchedContext &context)
{
	std::list<EventHandlerDataBase::TankRank>::iterator itor;
	for (itor = ranks_.begin();
		itor != ranks_.end();
		++itor)
	{
		EventHandlerDataBase::TankRank &rank = *itor;
		Tank *tank = context.getTargetContainer().getTankById(rank.getPlayerId());
		if (tank)
		{
			if (rank.getRank() != -1) tank->getScore().setRank(rank.getRank());
			if (rank.getSkill() != -1) tank->getScore().setSkill(rank.getSkill());
		}
	}

	return true;
}

bool TankRankSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) ranks_.size());
	std::list<EventHandlerDataBase::TankRank>::iterator itor;
	for (itor = ranks_.begin();
		itor != ranks_.end();
		++itor)
	{
		EventHandlerDataBase::TankRank &rank = *itor;
		buffer.addToBuffer(rank.getPlayerId());
		buffer.addToBuffer(rank.getSkill());
		buffer.addToBuffer(rank.getRank());
	}
	return true;
}

bool TankRankSimAction::readMessage(NetBufferReader &reader)
{
	int count = 0;
	if (!reader.getFromBuffer(count)) return false;
	for (int c=0; c<count; c++)
	{
		unsigned int playerId;
		int skill, rank;
		if (!reader.getFromBuffer(playerId)) return false;
		if (!reader.getFromBuffer(skill)) return false;
		if (!reader.getFromBuffer(rank)) return false;
		ranks_.push_back(EventHandlerDataBase::TankRank(playerId, rank, skill));
	}
	return true;
}
