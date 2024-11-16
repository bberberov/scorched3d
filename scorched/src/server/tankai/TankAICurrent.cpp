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

#include <tankai/TankAICurrent.h>
#include <tanket/TanketAccessories.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <common/OptionsTransient.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <XML/XMLNode.h>

TankAICurrent::TankAICurrent() : tanket_(0)
{
}

TankAICurrent::~TankAICurrent()
{
}

TankAI *TankAICurrent::createCopy(Tanket *tanket)
{
	TankAICurrent *result = new TankAICurrent(*this);
	result->setTanket(tanket);
	return result;
}

void TankAICurrent::setTanket(Tanket *tanket)
{
	tanket_ = tanket;
}

bool TankAICurrent::parseConfig(TankAIWeaponSets &sets, XMLNode *node)
{
	if (!TankAI::parseConfig(sets, node)) return false;

	{
		XMLNode *weapons = 0;
		if (!node->getNamedChild("weapons", weapons)) return false;
		if (!wantedWeapons_.parseConfig(sets, weapons)) return false;
	}
	{
		XMLNode *defense = 0;
		if (!node->getNamedChild("defense", defense)) return false;
		if (!defenses_.parseConfig(defense)) return false;
	}
	{
		XMLNode *attack = 0;
		if (!node->getNamedChild("attack", attack)) return false;
		if (!move_.parseConfig(attack)) return false;
	}

	return node->failChildren();	
}

void TankAICurrent::newMatch()
{
	newGame();
}

void TankAICurrent::newGame()
{
	move_.getTargets().clear();
	move_.clear();
}

void TankAICurrent::playMove(unsigned int moveId)
{
	// Raise any defenses
	defenses_.raiseDefenses(tanket_);

	// Make the move
	move_.playMove(tanket_, 
		wantedWeapons_.getCurrentWeaponSet(),
		defenses_.getUseBatteries(),
		moveId);
}

void TankAICurrent::buyAccessories(unsigned int moveId)
{
	bool lastRound = 
		(ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo() >=
		ScorchedServer::instance()->getOptionsGame().getNoRounds());

	TankAIWeaponSets::WeaponSetAccessories tankAccessories(tanket_);
	wantedWeapons_.buyWeapons(tankAccessories, lastRound);
	if (tankAccessories.tankAccessories.getAutoDefense().haveDefense())
	{
		defenses_.raiseDefenses(tanket_);
	}

	// This AI has finished buying
	ComsPlayedMoveMessage playedMessage(tanket_->getPlayerId(), 
		moveId, 
		ComsPlayedMoveMessage::eFinishedBuy);
	ScorchedServer::instance()->getServerState().buyingFinished(playedMessage);
}

void TankAICurrent::tankHurt(Weapon *weapon, float damage, 
	unsigned int damaged, unsigned int fired)
{
	if (damaged == tanket_->getPlayerId())
	{
		Tanket *firedTanket = ScorchedServer::instance()->
			getTargetContainer().getTanketById(fired);
		if (firedTanket)
		{
			move_.getTargets().tookDamage(firedTanket, damage);		
		}
	}
	else if (fired == tanket_->getPlayerId())
	{
		Tanket *damagedTanket = ScorchedServer::instance()->
			getTargetContainer().getTanketById(damaged);
		if (damagedTanket)
		{
			move_.getTargets().gaveDamage(damagedTanket, damage);		
		}
	}
}

void TankAICurrent::shotLanded(ScorchedCollisionId collision,
		Weapon *weapon, unsigned int firer, 
		Vector &position)
{
}
