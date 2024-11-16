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

#include <simactions/TankChangeSimAction.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <common/ChannelManager.h>
#include <tankai/TankAIStore.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelContainer.h>
#include <target/TargetContainer.h>
#include <tanket/TanketTypes.h>
#include <server/ScorchedServer.h>
#include <events/EventController.h>

unsigned int TankChangeSimAction::TankChangeSimActionCount = 0;

REGISTER_CLASS_SOURCE(TankChangeSimAction);

TankChangeSimAction::TankChangeSimAction()
{
	TankChangeSimActionCount++;
}

TankChangeSimAction::TankChangeSimAction(ComsTankChangeMessage &message) :
	message_(message)
{
	TankChangeSimActionCount++;
}

TankChangeSimAction::~TankChangeSimAction()
{
	TankChangeSimActionCount--;
}

bool TankChangeSimAction::invokeAction(ScorchedContext &context)
{
	// Validate player
	unsigned int playerId = message_.getPlayerId();
	Tank *tank = context.getTargetContainer().getTankById(playerId);
	if (!tank || tank->getState().getState() == TankState::sLoading)
	{
		return true;
	}

	// Check the tank is the correct AI
	if (0 != strcmp(message_.getPlayerType(), "Human"))
	{
		if (context.getServerMode())
		{
			TankAI *ai = ScorchedServer::instance()->getTankAIs().
				getAIByName(message_.getPlayerType());
			if (ai) tank->setTankAI(ai->createCopy(tank));
		}
		else
		{
			tank->setTankAI(0);
		}
		tank->setDestinationId(0);
	}
	else
	{
		tank->setDestinationId(message_.getDestinationId());
		tank->setTankAI(0);
	}

	// Make sure no-one has the same name
	LangString postFix = LANG_STRING("");
	LangString sentname = message_.getPlayerName();
	for (int p=1;;p++)
	{
		bool found = false;
		std::map<unsigned int, Tank *>::iterator mainitor;
		std::map<unsigned int, Tank *> tanks = 
			context.getTargetContainer().getTanks();
		for (mainitor = tanks.begin();
			mainitor != tanks.end();
			++mainitor)
		{
			Tank *currentTank = (*mainitor).second;
			if (currentTank->getTargetName() == (sentname + postFix) &&
				tank != currentTank) 
			{
				found = true;
				break;
			}
		}

		if (!found) break;
		postFix = LANG_STRING(S3D::formatStringBuffer("(%i)", p));
	}
	sentname += postFix;

	// Setup the new player
	LangString oldName = tank->getTargetName();
	tank->setName(sentname);

	// Player has set a new color
	if (tank->getTeam() == 0 &&
		message_.getPlayerColor() != tank->getColor())
	{
		// Check the color is not already in use
		std::map<unsigned int, Tank *> &tanks = 
			context.getTargetContainer().getTanks();
		if (TankColorGenerator::instance()->colorAvailable(
			message_.getPlayerColor(), tanks, tank))
		{
			// Set this color
			tank->setColor(message_.getPlayerColor());
		}
	}

	// Set avatar
	if (message_.getPlayerIconName()[0]) 
	{
		if (message_.getPlayerIcon().getBufferUsed() <=
			(unsigned) context.getOptionsGame().getMaxAvatarSize())
		{
			tank->getAvatar().setFromBuffer(
				message_.getPlayerIconName(),
				message_.getPlayerIcon());
		}
	}

	// Choose a team (if applicable)
	unsigned int oldTeam = tank->getTeam();
	if (context.getOptionsGame().getTeams() > 1)
	{
		if (message_.getPlayerTeam() > 0 && message_.getPlayerTeam() <=
			(unsigned int) context.getOptionsGame().getTeams())
		{
			tank->setTeam(message_.getPlayerTeam());
		}
		else
		{
			tank->setTeam(context.getOptionsTransient().getLeastUsedTeam(
				context.getTargetContainer()));
		}
	}

	// Tank type
	TanketType *type = context.getTanketTypes().getType(message_.getTankType());
	if (type)
	{
		tank->setTanketType(type);
	}

	// Make sure the model is available and for the correct team
	// Do this AFTER the team has been set
	tank->getModelContainer().setTankModelName(message_.getModelName());

	// Check what the tank is trying to do
	if (message_.getSpectate())
	{
		if (tank->getState().getState() != TankState::sSpectator)
		{
			ChannelManager::showText(
				context,
				ChannelText("info",
					"PLAYER_SPECTATOR",
					"Player spectating [p:{0}]",
					tank->getTargetName()));

			if (tank->getState().getState() == TankState::sNormal)
			{
				int scorePerSpectate = context.getOptionsGame().getScorePerSpectate();
				tank->getScore().setScore(
					tank->getScore().getScore() + scorePerSpectate);
			}

			// Set state
			tank->getState().setState(TankState::sSpectator);
			tank->getState().setNotSpectator(false);
			tank->getState().setLives(0);
		}
	}
	else 
	{
		if (tank->getState().getState() == TankState::sSpectator)
		{
			ChannelManager::showText(
				context,
				ChannelText("info",
					"PLAYER_PLAYING",
					"Player playing [p:{0}]",
					tank->getTargetName()));

			// Set state
			tank->getState().setState(TankState::sDead);
			tank->getState().setNotSpectator(true);
			tank->getState().setLives(0);

			// Tell the logger about a new tank
			context.getEventController().tankJoined(tank);
		}
		else
		{
			// The tank is not a spectator
			if (oldName != tank->getTargetName())
			{
				ChannelManager::showText(
					context,
					ChannelText("info",
						"PLAYER_NAME_CHANGE",
						"Player \"{0}\" changed name to \"[p:{1}]\"",
						oldName, tank->getTargetName()));
			}
			if (oldTeam != tank->getTeam())
			{
				ChannelManager::showText(
					context,
					ChannelText("info",
						"PLAYER_TEAM_CHANGE",
						"Player \"[p:{0}]\" changed to the {1} team",
						tank->getTargetName(),
						TankColorGenerator::getTeamName(tank->getTeam())));

				if (tank->getState().getState() == TankState::sNormal)
				{
					tank->getState().setState(TankState::sDead);
					tank->getState().setLives(0);
				}
			}
		}
	}

	return true;
}

bool TankChangeSimAction::writeMessage(NetBuffer &buffer)
{
	message_.writeMessage(buffer);
	return true;
}

bool TankChangeSimAction::readMessage(NetBufferReader &reader)
{
	if (!message_.readMessage(reader)) return false;
	return true;
}
