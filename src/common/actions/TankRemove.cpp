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

#include <actions/TankRemove.h>
#include <target/Target.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Logger.h>

TankRemove::TankRemove(unsigned int playerId, fixed removeTime) :
	Action(false),
	removeTime_(removeTime),
	playerId_(playerId)
{
}

TankRemove::~TankRemove()
{
	Target *target = context_->getTargetContainer().removeTarget(playerId_);
	if (!target) Logger::log("ERROR: Failed to find player to remove");
	delete target;
}

void TankRemove::init()
{
}

void TankRemove::simulate(fixed frameTime, bool &remove)
{
	removeTime_ -= frameTime;

	int alive = 0;
	std::map<unsigned int, Tank *> &tanks = context_->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = tanks.begin();
		mainitor != tanks.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying() &&
			current->getState().getLives() > 0 &&
			current->getPlayerId() != playerId_)
		{
			alive++;
		}
	}

	if (removeTime_ <= 0 || alive == 0)
	{
		remove = true;
	}

	Action::simulate(frameTime, remove);
}

std::string TankRemove::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s", playerId_, removeTime_.asQuickString());
}
