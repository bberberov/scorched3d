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

#include <simactions/PlayMovesSimAction.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <tanket/TanketAccessories.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankShotHistory.h>
#include <tank/TankState.h>
#include <target/TargetRenderer.h>
#include <tankai/TankAIStrings.h>
#include <actions/TankSay.h>
#include <actions/TanketResign.h>
#include <events/EventController.h>
#include <common/OptionsScorched.h>
#include <server/ServerCommon.h>
#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
#endif

REGISTER_CLASS_SOURCE(PlayMovesSimAction);

PlayMovesSimAction::PlayMovesSimAction() :
	moveId_(0)
{
}

PlayMovesSimAction::PlayMovesSimAction(unsigned int moveId, bool timeoutPlayers, bool referenced) :
	moveId_(moveId), timeoutPlayers_(timeoutPlayers), referenced_(referenced)
{
}

PlayMovesSimAction::~PlayMovesSimAction()
{
	while (!messages_.empty())
	{
		delete messages_.back();
		messages_.pop_back();
	}
}

void PlayMovesSimAction::addMove(ComsPlayedMoveMessage *message)
{
	messages_.push_back(message);
}

bool PlayMovesSimAction::invokeAction(ScorchedContext &context)
{
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		++itor)
	{
		ComsPlayedMoveMessage *message = *itor;
		Tanket *tanket = context.getTargetContainer().getTanketById(message->getPlayerId());
		if (tanket && tanket->getAlive())
		{
			switch (message->getType())
			{
			case ComsPlayedMoveMessage::eShot:
				tankFired(context, tanket, *message);
				break;
			case ComsPlayedMoveMessage::eResign:
				tankResigned(context, tanket, *message);
				break;
			}

			if (timeoutPlayers_ &&
				message->getType() == ComsPlayedMoveMessage::eTimeout)
			{
				tankTimedOut(context, tanket);
			}
			else
			{
				tanket->getShotInfo().setMissedMoves(0);
			}

			if (message->getType() == ComsPlayedMoveMessage::eTimeout ||
				message->getType() == ComsPlayedMoveMessage::eSkip)
			{
				tanket->getShotInfo().setSkippedShots(tanket->getShotInfo().getSkippedShots() + 1);
			}
			else
			{
				tanket->getShotInfo().setSkippedShots(0);
			}
		}
	}
	return true;
}

void PlayMovesSimAction::tankTimedOut(ScorchedContext &context, Tanket *tanket)
{
	if (!context.getServerMode()) return;

	int allowedMissed = 
		context.getOptionsGame().getAllowedMissedMoves();
	if (allowedMissed > 0)
	{
		tanket->getShotInfo().setMissedMoves(
			tanket->getShotInfo().getMissedMoves() + 1);

		if (tanket->getShotInfo().getMissedMoves() >= allowedMissed)
		{
			if (tanket->getType() == Target::TypeTank)
			{
				Tank *tank = (Tank *) tanket;
				ScorchedServer::instance()->getServerChannelManager().sendText(
					ChannelText("info",
						"PLAYER_MISSED_SPECTATOR",
						"[p:{0}] failed to move, moved to spectators",
						tank->getTargetName()),
						true);

				tank->getState().setState(TankState::sSpectator);
				tank->getState().setLives(0);
				tank->getState().setNotSpectator(false);
			}
		}
		else
		{
			ScorchedServer::instance()->getServerChannelManager().sendText(
				ChannelText("info",
					"PLAYER_MISSED_SHOOT",
					"[p:{0}] failed to move, allowed {1} more missed move(s)",
					tanket->getTargetName(),
					allowedMissed - tanket->getShotInfo().getMissedMoves()),
					true);
		}
	}
}

void PlayMovesSimAction::tankFired(ScorchedContext &context, 
	Tanket *tanket, ComsPlayedMoveMessage &message)
{
	// Check the weapon name exists and is a weapon
	Accessory *accessory = 
		context.getAccessoryStore().findByAccessoryId(message.getWeaponId());
	if (!accessory) return;

	Weapon *weapon = (Weapon *) accessory->getAction();
	if (accessory->getUseNumber() > 0)
	{
		// Actually use up one of the weapons
		// Fuel, is used up differently at the rate of one weapon per movement square
		// This is done sperately in the tank movement action
		tanket->getAccessories().rm(accessory, accessory->getUseNumber());
	}

	if (tanket->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) tanket;

		// Set the tank rotation so others can see what we are shooting at
#ifndef S3D_SERVER
		if (!context.getServerMode())
		{
			if (tank->getDestinationId() != context.getTargetContainer().getCurrentDestinationId())
			{

				tanket->getShotInfo().rotateGunXY(
					message.getRotationXY(), false);
				tanket->getShotInfo().rotateGunYZ(
					message.getRotationYZ(), false);
				tanket->getShotInfo().changePower(
					message.getPower(), false);
				tanket->getShotInfo().setSelectPosition(
					message.getSelectPositionX(), 
					message.getSelectPositionY());
			}
		}
#endif

		// Tank say
		if (tank->getDestinationId() == 0)
		{
			const char *line = context.getTankAIStrings().getAttackLine(context);
			if (line)
			{
				context.getActionController().addAction(
					new TankSay(tank->getPlayerId(), 
					LANG_STRING(line)));
			}
		}

		// Stats events
		context.getEventController().tankFired(tank, weapon);
	}

#ifndef S3D_SERVER
	if (!context.getServerMode()) 
	{
		TargetRenderer *renderer = tanket->getRenderer();
		if (renderer)
		{
			renderer->fired();
		}

		// play fired sound
		if (weapon->getParent()->getActivationSound() &&
			0 != strcmp("none", weapon->getParent()->getActivationSound()))
		{
			SoundBuffer *firedSound = 
				Sound::instance()->fetchOrCreateBuffer(
					S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", 
					weapon->getParent()->getActivationSound())));
			SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
				firedSound, tanket->getLife().getTargetPosition().asVector());
		}
	}
#endif // #ifndef S3D_SERVER

	// Get firing context
	FixedVector newVelocity = TankLib::getVelocityVector(
		message.getRotationXY(), message.getRotationYZ());
	WeaponFireContext weaponContext(
		tanket->getPlayerId(),
		message.getSelectPositionX(), message.getSelectPositionY(),
		newVelocity,
		referenced_, 
		(tanket->getType() == Target::TypeTank));
	FixedVector velocity = newVelocity * (message.getPower() + 1);
	FixedVector position = TankLib::getTankGunPosition(
		tanket->getLife().getTankTurretPosition(),
		message.getRotationXY(), message.getRotationYZ());

	// Create an action for the muzzle flash
	// add it to the action controller
	if (weapon->getParent()->getMuzzleFlash())
	{
		Accessory *muzzleFlash = context.getAccessoryStore().getMuzzelFlash();
		if (muzzleFlash) 
		{
			Weapon *muzzleWeapon = (Weapon *) muzzleFlash->getAction();
			muzzleWeapon->fire(context, weaponContext, position, velocity);
		}
	}

	// Create the action for the weapon and
	// add it to the action controller
	weapon->fire(context, weaponContext, position, velocity);
}

void PlayMovesSimAction::tankResigned(ScorchedContext &context, 
	Tanket *tanket, ComsPlayedMoveMessage &message)
{
	if (context.getOptionsGame().getResignMode() == OptionsGame::ResignNone)
	{
		return;
	}

	fixed resignTime = 0;
	if (context.getOptionsGame().getResignMode() == OptionsGame::ResignTimed)
	{
		resignTime = 10;
	}
	context.getActionController().addAction(
		new TanketResign(tanket->getPlayerId(), resignTime, referenced_));
}

bool PlayMovesSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(moveId_);
	buffer.addToBuffer(timeoutPlayers_);
	buffer.addToBuffer(referenced_);
	buffer.addToBuffer((unsigned int) messages_.size());
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		++itor)
	{
		ComsPlayedMoveMessage *message = *itor;
		message->writeMessage(buffer);
	}

	return true;
}

bool PlayMovesSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(moveId_)) return false;
	if (!reader.getFromBuffer(timeoutPlayers_)) return false;
	if (!reader.getFromBuffer(referenced_)) return false;
	unsigned int size = 0;
	if (!reader.getFromBuffer(size)) return false;
	for (unsigned int s=0; s<size; s++)
	{
		ComsPlayedMoveMessage *message = new ComsPlayedMoveMessage();
		if (!message->readMessage(reader)) return false;
		messages_.push_back(message);
	}
	return true;
}
