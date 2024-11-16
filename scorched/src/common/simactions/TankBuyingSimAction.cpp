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

#include <simactions/TankBuyingSimAction.h>
#include <placement/PlacementTankPosition.h>
#include <engine/Simulator.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>

std::set<unsigned int> TankBuyingSimAction::runningPlayerIds_;

REGISTER_CLASS_SOURCE(TankBuyingSimAction);

TankBuyingSimAction::TankBuyingSimAction() :
	playerId_(0),
	server_(false)
{
}

TankBuyingSimAction::TankBuyingSimAction(unsigned int playerId) :
	playerId_(playerId),
	server_(true)
{
	runningPlayerIds_.insert(playerId);
}

TankBuyingSimAction::~TankBuyingSimAction()
{
	if (server_) runningPlayerIds_.erase(playerId_);
}

bool TankBuyingSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTargetContainer().getTankById(playerId_);
	if (!tank) return false;

	if (tank->getState().getState() != TankState::sDead) return true;
	tank->getState().setState(TankState::sBuying);

	FixedVector tankPos = PlacementTankPosition::placeTank(
		tank->getPlayerId(), tank->getTeam(),
		context,
		context.getSimulator().getRandomGenerator());
	tank->getLife().setTargetPosition(tankPos);

	return true;
}

bool TankBuyingSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	return true;
}

bool TankBuyingSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	return true;
}