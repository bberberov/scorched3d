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

#include <math.h>
#include <tank/Tank.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelStore.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankShotHistory.h>
#include <tank/TankModelContainer.h>
#include <tank/TankViewPoints.h>
#include <tank/TankAvatar.h>
#include <tank/TankCamera.h>
#include <tank/TankWeaponSwitcher.h>
#include <tankai/TankAI.h>
#include <tankai/TankAIStore.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <algorithm>

static TankWeaponSwitcher *weaponSwitcher = new TankWeaponSwitcher();

Tank::Tank(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const LangString &name, 
		Vector &color) :
	Tanket(context, playerId, destinationId, name), 
	context_(context),
	color_(color), 
	ipAddress_(0)
{
	score_ = new TankScore(context);
	state_ = new TankState(context, playerId);
	shotHistory_ = new TankShotHistory(context);
	modelContainer_ = new TankModelContainer(context_);
	avatar_ = new TankAvatar();
	camera_ = new TankCamera(context);
	viewPoints_ = new TankViewPointsCollection(context_);

	getAccessories().getWeapons().setWeaponSwitcher(weaponSwitcher);

	shotHistory_->setTank(this);
	score_->setTank(this);
	state_->setTank(this);
	modelContainer_->setTank(this);
	state_->setState(TankState::sLoading);
}

Tank::~Tank()
{
	delete score_; score_ = 0;
	delete state_; state_ = 0;
	delete shotHistory_; shotHistory_ = 0;
	delete modelContainer_; modelContainer_ = 0;
	delete avatar_; avatar_ = 0;
	delete camera_; camera_ = 0;
	delete viewPoints_; viewPoints_ = 0;
}

void Tank::newMatch()
{
	Tanket::newMatch();
	score_->newMatch();
	state_->newMatch();
}

void Tank::newGame()
{
	Tanket::newGame();

	state_->newGame();
	score_->newGame();
}

void Tank::rezTank()
{
	if (getTankAI()) getTankAI()->newGame();
	getState().setState(TankState::sNormal);
	getLife().setLife(getLife().getMaxLife());
	shotHistory_->undo();
}

void Tank::clientNewGame()
{
	shotHistory_->clientNewGame();
	state_->clientNewGame();
}

bool Tank::getAlive()
{
	return getState().getState() == TankState::sNormal;
}

bool Tank::getVisible()
{
	return getAlive() || getState().getState() == TankState::sBuying;
}

bool Tank::getPlaying()
{
	return getState().getTankPlaying();
}

static inline bool lt_accessory(const Accessory *o1, const Accessory *o2) 
{ 
	return ((Accessory*)o1)->getAccessoryId() > ((Accessory *)o2)->getAccessoryId();
}

Accessory *Tank::getDeathAction()
{
	std::list<Accessory *> &accessories = getAccessories().
		getAllAccessoriesByGroup("deathaction");
	if (!accessories.empty())
	{
		std::vector<Accessory *> accessoriesVector;
		accessoriesVector.insert(accessoriesVector.begin(), accessories.begin(), accessories.end());
		std::sort(accessoriesVector.begin(), accessoriesVector.end(), lt_accessory);

		Accessory *accessory = accessoriesVector[
			context_.getSimulator().getRandomGenerator().getRandUInt("getDeathAction") % accessoriesVector.size()];
		if (accessory->getAction()->getType() == AccessoryPart::AccessoryWeapon)
		{
			return accessory;
		}
	}

	return context_.getAccessoryStore().getDeathAnimation();
}

Vector &Tank::getColor()
{
	if (getTeam() > 0) return TankColorGenerator::getTeamColor(getTeam());
	return color_;
}

bool Tank::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "Tank");

	if (!Tanket::writeMessage(buffer)) return false;  // Base class 1st
	buffer.addToBufferNamed("color", color_);
	if (!state_->writeMessage(buffer)) return false;
	if (!score_->writeMessage(buffer)) return false;
	if (!modelContainer_->writeMessage(buffer)) return false;
	if (!avatar_->writeMessage(buffer)) return false;
	return true;
}

bool Tank::readMessage(NetBufferReader &reader)
{
	if (!Tanket::readMessage(reader)) 
	{
		Logger::log("Tanket::readMessage failed");
		return false; // Base class 1st
	}
	if (!reader.getFromBuffer(color_))
	{
		Logger::log("Tank::color_ read failed");
		return false;
	}
	if (!state_->readMessage(reader))
	{
		Logger::log("Tank::state_ read failed");
		return false;
	}
	if (!score_->readMessage(reader))
	{
		Logger::log("Tank::score_ read failed");
		return false;
	}
	if (!modelContainer_->readMessage(reader))
	{
		Logger::log("Tank::modelContainer_ read failed");
		return false;
	}
	if (!avatar_->readMessage(reader))
	{
		Logger::log("Tank::avatar_ read failed");
		return false;
	}

	return true;
}
