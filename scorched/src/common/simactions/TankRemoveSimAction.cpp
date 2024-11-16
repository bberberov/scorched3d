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

#include <simactions/TankRemoveSimAction.h>
#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <tankai/TankAINone.h>
#include <actions/TankRemove.h>

unsigned int TankRemoveSimAction::TankRemoveSimActionCount = 0;

REGISTER_CLASS_SOURCE(TankRemoveSimAction);

TankRemoveSimAction::TankRemoveSimAction()
{
	TankRemoveSimActionCount++;
}

TankRemoveSimAction::TankRemoveSimAction(unsigned int playerId, fixed removalTime) :
	playerId_(playerId), removalTime_(removalTime)
{
	TankRemoveSimActionCount++;
}

TankRemoveSimAction::~TankRemoveSimAction()
{
	TankRemoveSimActionCount--;
}

bool TankRemoveSimAction::invokeAction(ScorchedContext &context)
{
	Tank *tank = context.getTargetContainer().getTankById(playerId_);
	if (tank)
	{
		// Make this player a computer controlled player
		if (context.getServerMode())
		{
			TankAI *ai = new TankAINone(tank->getPlayerId());
			tank->setTankAI(ai); 
		}
		tank->setDestinationId(0);
	}

	TankRemove *removeAction = new TankRemove(playerId_, removalTime_);
	context.getActionController().addAction(removeAction);
	return true;
}

bool TankRemoveSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(removalTime_);
	return true;
}

bool TankRemoveSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(removalTime_)) return false;
	return true;
}
