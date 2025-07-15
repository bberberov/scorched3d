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

#include <graph/SpeedChange.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <server/ScorchedServer.h>
#include <common/Keyboard.h>
#include <common/Defines.h>
#include <client/ClientParams.h>
#include <engine/Simulator.h>
#include <graph/ParticleEngine.h>
#include <GLW/GLWFont.h>
#include <stdio.h>

SpeedChange *SpeedChange::instance_ = 0;

SpeedChange *SpeedChange::instance()
{
	if (!instance_)
	{
		instance_ = new SpeedChange;
	}
	return instance_;
}

SpeedChange::SpeedChange() :
	GameStateI("SpeedChange")
{
}

SpeedChange::~SpeedChange()
{
}

void SpeedChange::resetSpeed()
{
	setSpeed(1);
}

void SpeedChange::draw(const unsigned state)
{
	float speed = ScorchedClient::instance()->getSimulator().getFast().asFloat();
	if (speed != 1.0f)
	{
		GLState state(GLState::BLEND_ON | GLState::TEXTURE_OFF); 
		static char buffer[10];
		snprintf(buffer, 10, "%.1fX", speed);

		static Vector fontColor(0.7f, 0.7f, 0.2f);
		GLWFont::instance()->getGameFont()->draw(fontColor, 20, 10.0f, 10.0f, 0.0f, buffer);
	}
}

void SpeedChange::simulate(const unsigned state, float simTime)
{
	fixed speed = ScorchedClient::instance()->getSimulator().getFast();
	float timeTaken = frameClock_.getTimeDifference();
	if (timeTaken > 0.2f && speed > 1)
	{
		setSpeed(speed / 2);
	}
}

void SpeedChange::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest)
{
	if (ClientParams::instance()->getConnectedToServer()) return;

	KEYBOARDKEY("SIMULATION_SPEED_X1", x1Key);
	KEYBOARDKEY("SIMULATION_SPEED_X2", x2Key);
	KEYBOARDKEY("SIMULATION_SPEED_X3", x3Key);
	KEYBOARDKEY("SIMULATION_SPEED_X4", x4Key);
	KEYBOARDKEY("SIMULATION_SPEED_HALF", xHalfKey);
	KEYBOARDKEY("SIMULATION_SPEED_QUARTER", xQuarterKey);
	KEYBOARDKEY("SIMULATION_SPEED_EIGHTH", xEighthKey);

	if (x1Key->keyDown(buffer, keyState))
	{
		resetSpeed();
	}
	else if (x2Key->keyDown(buffer, keyState))
	{
		setSpeed(2);
	}
	else if (x3Key->keyDown(buffer, keyState))
	{
		setSpeed(4);
	}
	else if (x4Key->keyDown(buffer, keyState))
	{
		setSpeed(8);
	}
	else if (xHalfKey->keyDown(buffer, keyState))
	{
		setSpeed(fixed(1) / fixed(2));
	}
	else if (xQuarterKey->keyDown(buffer, keyState))
	{
		setSpeed(fixed(1) / fixed(4));
	}
	else if (xEighthKey->keyDown(buffer, keyState))
	{
		setSpeed(fixed(1) / fixed(8));
	}
}

void SpeedChange::setSpeed(fixed speed)
{
	ScorchedClient::instance()->getSimulator().setFast(speed);
	ParticleEngine::setFast(speed.asFloat());

	if (!ClientParams::instance()->getConnectedToServer() &&
		ScorchedServer::serverStarted())
	{
		ScorchedServer::instance()->getSimulator().setFast(speed);
	}
}
