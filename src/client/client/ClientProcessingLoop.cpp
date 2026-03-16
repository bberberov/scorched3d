////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#include <client/ClientProcessingLoop.hpp>
#include <client/ClientParams.hpp>
#include <client/ScorchedClient.hpp>
#include <client/ClientMain.hpp>
#include <net/NetInterface.hpp>
#include <common/Logger.hpp>
#include <target/TargetContainer.hpp>
#include <server/ServerMain.hpp>
#include <engine/Simulator.hpp>

time_t startTime = 0;

ClientProcessingLoop *ClientProcessingLoop::instance_ = nullptr;

ClientProcessingLoop *ClientProcessingLoop::instance()
{
	if (nullptr == instance_)
	{
		instance_ = new ClientProcessingLoop();
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

	if ( 0 < ClientParams::instance()->getExitTime() )
	{
		if ( 0 == startTime ) startTime = time( nullptr );
		if ( ClientParams::instance()->getExitTime() < time( nullptr ) - startTime )
		{
			exit( 0 );
		}
	}
	if ( 0 < ClientParams::instance()->getDisconnectTime() )
	{
		if ( 0 == startTime ) startTime = time( nullptr );
		if ( ClientParams::instance()->getDisconnectTime() < time( nullptr ) - startTime )
		{
			startTime = time( nullptr );
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
