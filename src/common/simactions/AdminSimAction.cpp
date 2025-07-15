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

#include <simactions/AdminSimAction.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>

REGISTER_CLASS_SOURCE(AdminSimAction);

AdminSimAction::AdminSimAction()
{
}

AdminSimAction::AdminSimAction(AdminType type, unsigned int playerId, fixed amount) :
	type_(type), playerId_(playerId), amount_(amount)
{
}

AdminSimAction::AdminSimAction(unsigned int playerId, const LangString &newName) :
	type_(eChangeName), playerId_(playerId), amount_(0), newName_(newName)
{
}

AdminSimAction::~AdminSimAction()
{
}

bool AdminSimAction::invokeAction(ScorchedContext &context)
{
	switch (type_)
	{
	case eKillAll:
		killAll(context);
		break;
	case eNewGame:
		newGame(context);
		break;
	case eSlap:
		slap(context);
		break;
	case eKill:
		kill(context);
		break;
	case eChangeName:
		changeName(context);
		break;
	}

	return true;
}

bool AdminSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) type_);
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(amount_);
	buffer.addToBuffer(newName_);
	return true;
}

bool AdminSimAction::readMessage(NetBufferReader &reader)
{
	int type;
	if (!reader.getFromBuffer(type)) return false;
	type_ = (AdminType) type;
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(amount_)) return false;
	if (!reader.getFromBuffer(newName_)) return false;
	return true;
}

void AdminSimAction::killAll(ScorchedContext &context)
{
	Logger::log("Killing all players");

	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tanks = 
		context.getTargetContainer().getTanks();
	for (itor = tanks.begin();
		 itor != tanks.end();
		 ++itor)
	{
		Tank *current = (*itor).second;
		if (current->getState().getState() == TankState::sNormal ||
			current->getState().getState() == TankState::sDead ||
			current->getState().getState() == TankState::sBuying)
		{
			current->getState().setState(TankState::sDead);
			current->getState().setLives(0);
		}
	}
}

void AdminSimAction::newGame(ScorchedContext &context)
{
	killAll(context);
	context.getOptionsTransient().startNewGame();	
}

void AdminSimAction::changeName(ScorchedContext &context)
{
	Tank *targetTank = context.
		getTargetContainer().getTankById(playerId_);
	if (!targetTank) return;

	targetTank->setName(newName_);
}

void AdminSimAction::slap(ScorchedContext &context)
{
	Tank *targetTank = context.
		getTargetContainer().getTankById(playerId_);
	if (!targetTank) return;

	if (targetTank->getState().getState() == TankState::sNormal ||
		targetTank->getState().getState() == TankState::sDead ||
		targetTank->getState().getState() == TankState::sBuying)
	{
		targetTank->getLife().setLife(
			targetTank->getLife().getLife() -  amount_);
	}
}

void AdminSimAction::kill(ScorchedContext &context)
{
	Tank *targetTank = context.
		getTargetContainer().getTankById(playerId_);
	if (!targetTank) return;

	if (targetTank->getState().getState() == TankState::sNormal ||
		targetTank->getState().getState() == TankState::sDead ||
		targetTank->getState().getState() == TankState::sBuying)
	{
		targetTank->getState().setState(TankState::sDead);
		targetTank->getState().setLives(0);
	}
}
