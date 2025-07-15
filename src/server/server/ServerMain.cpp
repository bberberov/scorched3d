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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <weapons/AccessoryStore.h>
#include <weapons/EconomyStore.h>
#include <net/NetLoopBack.h>
#include <net/NetServerTCP3.h>
#include <lua/LUAScriptHook.h>
#include <common/Defines.h>
#include <common/Clock.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <engine/Simulator.h>
#include <engine/ModFiles.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStore.h>
#include <target/TargetContainer.h>
#include <server/ServerConsoleProgressCounter.h>
#include <server/ServerConsoleLogger.h>
#include <server/ServerConnectAuthHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerFileServer.h>
#include <server/ServerLoadLevel.h>
#include <server/ServerRegistration.h>
#include <server/ServerLog.h>
#include <server/ServerBrowserInfo.h>
#include <server/ServerCommon.h>
#include <server/ServerBanned.h>
#include <server/ServerMain.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <server/ServerTimedMessage.h>
#include <server/ServerParams.h>
#include <SDL/SDL.h>

#include <igd/igd.h>

#ifdef S3D_SERVER
#include <webserver/ServerWebServer.h>
#endif

static Clock serverTimer;

static void serverMain(ProgressCounter *counter)
{
	ScorchedServerSettingsOptions settings(
		ServerParams::instance()->getServerFile(),
		ServerParams::instance()->getRewriteOptions(),
		ServerParams::instance()->getWriteFullOptions());

	// Create the server states
	if (!ScorchedServer::instance()->startServer(
		settings,
		false, counter)) exit(64);

	// Try to start the server
	if (!ScorchedServer::instance()->getContext().getNetInterface().start(
		ScorchedServer::instance()->getOptionsGame().getPortNo()) ||
		!ServerBrowserInfo::instance()->start())
	{
		S3D::dialogExit("Scorched3D Server", 
			S3D::formatStringBuffer("Failed to start the server.\n\n"
			"Could not bind to the server ports.\n"
			"Ensure the specified ports (%i, %i) do not conflict with any other program.",
			ScorchedServer::instance()->getOptionsGame().getPortNo(),
			ScorchedServer::instance()->getOptionsGame().getPortNo() + 1));
	}

	// Try to update the route to add the external port mapping
	if (ScorchedServer::instance()->getOptionsGame().getUseUPnP())
	{
		igd pnpigd;
		pnpigd.sendInitialRequest(ScorchedServer::instance()->getOptionsGame().getPortNo());
	}

	// Contact the registration server
 	if (ScorchedServer::instance()->getOptionsGame().getPublishServer()) 
	{
		ServerRegistration::instance()->start();
	}

#ifdef S3D_SERVER
	if (ScorchedServer::instance()->getOptionsGame().getManagementPortNo() > 0)
	{
		ServerWebServer::instance()->start(
			ScorchedServer::instance()->getOptionsGame().getManagementPortNo());

		Logger::log(S3D::formatStringBuffer("Management server running on url http://127.0.0.1:%i",
			ScorchedServer::instance()->getOptionsGame().getManagementPortNo()));
	}
#endif
	ServerLog::instance();

	std::string startTime = S3D::getStartTime();
	Logger::log(S3D::formatStringBuffer("Server started : %s", startTime.c_str()));
}

void serverLoop(fixed timeDifference)
{
	Logger::processLogEntries();

	// Main server loop:
	if (!ScorchedServer::serverStarted() ||
		!ScorchedServer::instance()->getContext().getNetInterfaceValid())
	{
		return;
	}
		
		ScorchedServer::instance()->getNetInterface().processMessages();
#ifdef S3D_SERVER
		{
			ServerBrowserInfo::instance()->processMessages();
			ServerWebServer::instance()->processMessages();
		}
#endif

		ScorchedServer::instance()->getSimulator().simulate();
		ScorchedServer::instance()->getServerState().simulate(timeDifference);

		ScorchedServer::instance()->getServerConnectAuthHandler().processMessages();
		ScorchedServer::instance()->getServerFileServer().simulate();
		ScorchedServer::instance()->getServerChannelManager().simulate(timeDifference);
		ScorchedServer::instance()->getTimedMessage().simulate();

		if (timeDifference > 5)
		{
			Logger::log(S3D::formatStringBuffer("Warning: Server loop took %.2f seconds", 
				timeDifference.asFloat()));
		}
}

void consoleServer()
{
	ServerConsoleProgressCounter::instance();
	ServerConsoleLogger serverConsoleLogger;
	ServerCommon::startFileLogger(ServerParams::instance()->getServerFile());
	serverMain(ServerConsoleProgressCounter::instance()->getProgressCounter());

	serverTimer.getTicksDifference();
	for (;;)
	{
		SDL_Delay(10);

		unsigned int ticksDifference = serverTimer.getTicksDifference();
		fixed timeDifference(true, ticksDifference * 10);
		serverLoop(timeDifference);

		if (ServerParams::instance()->getExitTime() > 0)
		{
			if (time(0) > ServerParams::instance()->getExitTime())
			{
				Logger::log("Exiting server due to exit time");
				Logger::processLogEntries();
				break;
			}
		}
	}

	ScorchedServer::instance()->stopServer();
}
