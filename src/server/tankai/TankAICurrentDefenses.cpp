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

#include <tankai/TankAICurrentDefenses.h>
#include <tank/Tank.h>
#include <tanket/TanketAccessories.h>
#include <weapons/Accessory.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <coms/ComsDefenseMessage.h>
#include <simactions/TankDefenseSimAction.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <XML/XMLNode.h>
#include <stdlib.h>

TankAICurrentDefenses::TankAICurrentDefenses()
{
}


TankAICurrentDefenses::~TankAICurrentDefenses()
{
}

bool TankAICurrentDefenses::parseConfig(XMLNode *node)
{
	if (!node->getNamedChild("useparachutes", useParachutes_)) return false;
	if (!node->getNamedChild("useshields", useShields_)) return false;
	if (!node->getNamedChild("usebatteries", useBatteries_)) return false;
	return node->failChildren();	
}

void TankAICurrentDefenses::selectFirstShield(Tanket *tanket)
{
	if (tanket->getShield().getCurrentShield()) return;

	std::list<Accessory *> &shields =
		tanket->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryShield);
	if (!shields.empty())
	{
		Accessory *shield = shields.front();
		shieldsUpDown(tanket, shield->getAccessoryId());
	}
}

void TankAICurrentDefenses::selectFirstParachute(Tanket *tanket)
{
	if (tanket->getParachute().getCurrentParachute()) return;

	std::list<Accessory *> &parachutes =
		tanket->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryParachute);
	if (!parachutes.empty())
	{
		Accessory *parachute = parachutes.front();
		parachutesUpDown(tanket, parachute->getAccessoryId());
	}
}

void TankAICurrentDefenses::raiseDefenses(Tanket *tanket)
{
	// Try to enable parachutes (fails if we don't have any)
	if (useParachutes_) selectFirstParachute(tanket);

	// Try to raise shields (fails if we don't have any)
	if (useShields_) selectFirstShield(tanket);
}

void TankAICurrentDefenses::parachutesUpDown(Tanket *tanket, unsigned int paraId)
{
	ComsDefenseMessage defenseMessage(
		tanket->getPlayerId(),
		(paraId!=0)?ComsDefenseMessage::eParachutesUp:ComsDefenseMessage::eParachutesDown,
		paraId);

	TankDefenseSimAction *simAction = new TankDefenseSimAction(defenseMessage);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}

void TankAICurrentDefenses::shieldsUpDown(Tanket *tanket, unsigned int shieldId)
{
	ComsDefenseMessage defenseMessage(
		tanket->getPlayerId(),
		(shieldId!=0)?ComsDefenseMessage::eShieldUp:ComsDefenseMessage::eShieldDown,
		shieldId);

	TankDefenseSimAction *simAction = new TankDefenseSimAction(defenseMessage);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
}
