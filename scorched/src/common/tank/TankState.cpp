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

#include <stdio.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <engine/ScorchedContext.h>
#include <lang/LangResource.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>

static struct AllowedStateTransitions
{
	TankState::State from, to;
}
allowedStateTransitions[] =
{
	{ TankState::sLoading, TankState::sSpectator },
	{ TankState::sLoading, TankState::sDead },
	{ TankState::sSpectator, TankState::sNormal },
	{ TankState::sSpectator, TankState::sDead },
	{ TankState::sDead, TankState::sNormal },
	{ TankState::sDead, TankState::sBuying },
	{ TankState::sDead, TankState::sLoading },
	{ TankState::sDead, TankState::sSpectator },
	{ TankState::sNormal, TankState::sDead },
	{ TankState::sNormal, TankState::sLoading },
	{ TankState::sNormal, TankState::sSpectator },
	{ TankState::sSpectator, TankState::sLoading },
	{ TankState::sBuying, TankState::sSpectator },
	{ TankState::sBuying, TankState::sLoading },
	{ TankState::sBuying, TankState::sNormal }
};

TankState::TankState(ScorchedContext &context, unsigned int playerId) : 
	state_(sLoading), tank_(0),
	context_(context), 
	muted_(false),
	skipshots_(false),
	lives_(0), maxLives_(1),
	notSpectator_(false),
	newlyJoined_(true),
	stateChangeCount_(0)
{
}

TankState::~TankState()
{
}

void TankState::newMatch()
{
	stateChangeCount_ = 0;
	setState(sDead);
}

void TankState::newGame()
{
	setState(sNormal);

	maxLives_ = context_.getOptionsGame().getPlayerLives();

	lives_ = maxLives_;
	tank_->getTargetState().setFalling(0);
	tank_->getTargetState().setMoving(0);
}

void TankState::clientNewGame()
{
	skipshots_ = false;
}

void TankState::setState(State s)
{
	for (int i=0; i<sizeof(allowedStateTransitions) / 
			sizeof(AllowedStateTransitions); i++)
	{
		if (state_ == allowedStateTransitions[i].from &&
			s == allowedStateTransitions[i].to)
		{
			state_ = s;
			stateChangeCount_++;
			break;
		}
	}

 	if (state_ != sNormal && state_ != sBuying)
	{
		// Make sure the target and shield physics
		// are disabled
		tank_->getLife().setLife(0);
		tank_->getShield().setCurrentShield(0);
	}
	else
	{
		// Make sure target space contains tank
		tank_->getLife().setLife(tank_->getLife().getLife());
	}
}

const char *TankState::getStateString()
{
	static char string[1024];
	snprintf(string, 1024, "%s %s (%i hp)",
		getSmallStateString(),
		(muted_?"muted ":""),
		(int) tank_->getLife().getLife().asInt());
	return string;
}

const char *TankState::getSmallStateString()
{
	const char *type = "";
	switch (state_)
	{
	case sNormal:
		type = "Alive";
		break;
	case sDead:
		type = "Dead";
		break;
	case sSpectator:
		type = "Spectator";
		break;
	case sLoading:
		type = "Loading";
		break;
	case sBuying:
		type = "Buying";
		break;
	}

	return type;
}

LangString &TankState::getSmallStateLangString()
{
	LANG_RESOURCE_CONST_VAR(DEAD, "DEAD", "Dead");
	LANG_RESOURCE_CONST_VAR(ALIVE, "ALIVE", "Alive");
	LANG_RESOURCE_CONST_VAR(SPECTATOR, "SPECTATOR", "Spectator");
	LANG_RESOURCE_CONST_VAR(LOADING, "LOADING", "Loading");
	LANG_RESOURCE_CONST_VAR(BUYING, "BUYING", "Buying");

	switch (state_)
	{
	case sNormal:
		return ALIVE;
	case sDead:
		return DEAD;
	case sSpectator:
		return SPECTATOR;
	case sLoading:
		return LOADING;
	case sBuying:
		return BUYING;
	}

	static LangString nullResult;
	return nullResult;
}

bool TankState::getTankPlaying()
{
	return state_ == sNormal || state_ == sDead || state_ == sBuying;
}

bool TankState::getTankAliveOrBuying()
{
	return state_ == sNormal || state_ == sBuying;
}

bool TankState::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankState");

	buffer.addToBufferNamed("state", (int) state_);
	buffer.addToBufferNamed("lives", lives_);
	buffer.addToBufferNamed("maxLives", maxLives_);
	buffer.addToBufferNamed("newlyjoined", newlyJoined_);
	buffer.addToBufferNamed("notspectator", notSpectator_);
	return true;
}

bool TankState::readMessage(NetBufferReader &reader)
{
	int s;
	if (!reader.getFromBuffer(s))
	{
		Logger::log("TankState::state_ read failed");
		return false;
	}
	state_ = (TankState::State) s;
	setState((TankState::State) s);
	if (!reader.getFromBuffer(lives_))
	{
		Logger::log("TankState::lives_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(maxLives_))
	{
		Logger::log("TankState::maxLives_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(newlyJoined_))
	{
		Logger::log("TankState::newlyJoined_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(notSpectator_))
	{
		Logger::log("TankState::notSpectator_ read failed");
		return false;
	}
	return true;
}
