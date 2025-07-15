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

#include <client/ClientProcessingLoop.h>
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#include <client/ClientMain.h>
#include <net/NetInterface.h>
#include <common/Logger.h>
#include <target/TargetContainer.h>
#include <server/ServerMain.h>
#include <engine/Simulator.h>

time_t startTime = 0;

ClientProcessingLoop *ClientProcessingLoop::instance_ = 0;

ClientProcessingLoop *ClientProcessingLoop::instance()
{
	if (!instance_)
	{
		instance_ = new ClientProcessingLoop;
	}
	return instance_;
}

ClientProcessingLoop::ClientProcessingLoop() :
	GameStateI("ClientProcessingLoop"),
	serverTime_(0.0f)
{
}

ClientProcessingLoop::~ClientProcessingLoop()
{
}

void ClientProcessingLoop::simulate(const unsigned state, float frameTime)
{
	process(frameTime, true);
}

void ClientProcessingLoop::process(float frameTime, bool processClientMessages)
{
	if (!ClientParams::instance()->getConnectedToServer())
	{
		serverTime_ += frameTime;
		if (serverTime_ > 0.05f)
		{
			fixed timeDifference = fixed::fromFloat(serverTime_);
			timeDifference *= ScorchedClient::instance()->getSimulator().getFast();
			serverLoop(timeDifference);
			serverTime_ = 0.0f;
		}
	}

	Logger::processLogEntries();
	if (processClientMessages &&
		ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getNetInterface().processMessages();
	}

	if (ClientParams::instance()->getExitTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getExitTime())
		{
			exit(0);
		}
	}
	if (ClientParams::instance()->getDisconnectTime() > 0)
	{
		if (startTime == 0) startTime = time(0);
		if (time(0) - startTime > ClientParams::instance()->getDisconnectTime())
		{
			startTime = time(0);
			if (ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
			{
				ScorchedClient::instance()->getNetInterface().disconnectAllClients();
			}
			else
			{
				ClientMain::startClient();
			}
		}
	}
}

void ClientProcessingLoop::draw(const unsigned state)
{
	// Make sure frame rate is not exceeded
	limiter_.limitFrameTime(); 
}
