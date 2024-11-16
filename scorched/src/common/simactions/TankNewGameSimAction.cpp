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

#include <simactions/TankNewGameSimAction.h>
#include <placement/PlacementTankPosition.h>
#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/Simulator.h>
#include <engine/ScorchedContext.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>

unsigned int TankNewGameSimAction::instanceCount_(0);

REGISTER_CLASS_SOURCE(TankNewGameSimAction);

TankNewGameSimAction::TankNewGameSimAction() :
	playerId_(0)
{
	instanceCount_++;
}

TankNewGameSimAction::TankNewGameSimAction(unsigned int playerId) :
	playerId_(playerId)
{
	instanceCount_++;
}

TankNewGameSimAction::~TankNewGameSimAction()
{
	instanceCount_--;
}

bool TankNewGameSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTargetContainer().getTankById(playerId_);
	if (!tank) return false;

	if (tank->getState().getState() != TankState::sBuying) return true;

	FixedVector tankPos = tank->getLife().getTargetPosition();
	tankPos[2] = context.getLandscapeMaps().getGroundMaps().getInterpHeight(tankPos[0], tankPos[1]);

	DeformLandscape::flattenArea(context, tankPos);

	tank->newGame();
	if (!context.getServerMode()) tank->clientNewGame();

	return true;
}

bool TankNewGameSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	return true;
}

bool TankNewGameSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	return true;
}