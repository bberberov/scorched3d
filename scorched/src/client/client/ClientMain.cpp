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

#include <server/ServerMain.h>
#include <client/ClientMain.h>
#include <client/ClientDialog.h>
#include <client/ScorchedClient.h>
#include <client/ClientAdmin.h>
#include <client/ClientParams.h>
#include <client/ClientChannelManager.h>
#include <client/ClientGameStoppedHandler.h>
#include <client/ClientMessageHandler.h>
#include <client/ClientConnectionRejectHandler.h>
#include <client/ClientLinesHandler.h>
#include <client/ClientStartGameHandler.h>
#include <client/ClientProcessingLoop.h>
#include <client/ClientConnectionAcceptHandler.h>
#include <client/ClientConnectionAuthHandler.h>
#include <client/ClientOperationHandler.h>
#include <client/ClientLoadLevelHandler.h>
#include <client/ClientInitializeModHandler.h>
#include <client/ClientAdminResultHandler.h>
#include <client/ClientFileHandler.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <lang/LangResource.h>
#include <graph/Mouse.h>
#include <graph/Gamma.h>
#include <graph/OptionsDisplay.h>
#include <graph/OptionsDisplayConsole.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/Display.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/AnimatedBackdropDialog.h>
#include <dialogs/BackdropDialog.h>
#include <dialogs/ConnectDialog.h>
#include <server/ScorchedServer.h>
#include <console/ConsoleFileReader.h>
#include <console/Console.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <engine/MainLoop.h>
#include <engine/ActionController.h>
#include <dialogs/ProgressDialog.h>
#include <net/NetServerTCP.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>
#include <common/ARGParser.h>
#include <common/Keyboard.h>
#include <common/OptionsScorched.h>
#include <common/Keyboard.h>
#include <common/ProgressCounter.h>
#include <common/Clock.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <sound/Sound.h>
#include <SDL/SDL.h>

static bool paused = false;
extern char scorched3dAppName[128];

static bool initHardware(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp(LANG_RESOURCE("INITIALIZING_KEYBOARD", "Initializing Keyboard"));
	if (!Keyboard::instance()->init())
	{
		S3D::dialogMessage("Scorched3D Keyboard", 
			"SDL failed to aquire keyboard.\n"
#ifdef WIN32
			"Is DirectX 5.0 installed?"
#endif
		);
		return false;
	}
	progressCounter->setNewOp(LANG_RESOURCE("LOADING_KEYBOARD", "Loading Keyboard Bindings"));
	if (!Keyboard::instance()->loadKeyFile())
	{
		S3D::dialogMessage("Scorched3D Keyboard", 
			"Failed to process keyboard file keys.xml");
		return false;
	}

	if (!OptionsDisplay::instance()->getNoSound())
	{
		progressCounter->setNewOp(LANG_RESOURCE("INITIALIZING_SOUND", "Initializing Sound"));
		if (!Sound::instance()->init(
			OptionsDisplay::instance()->getSoundChannels()))
		{
			S3D::dialogMessage("Scorched3D Sound", 
				"Failed to aquire sound.\n"
				"Is anything else currently using the sound card?");
		}
		else
		{
			Sound::instance()->getDefaultListener()->setGain(
				float(OptionsDisplay::instance()->getSoundVolume()) / 128.0f);
		}
	}
	return true;
}

static bool initComs(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp(LANG_RESOURCE("INITIALIZING_COMS", "Initializing Coms"));
	ScorchedClient::instance();

	// Tidy up any existing net handlers
	if (ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getContext().getNetInterface().stop();
		delete &ScorchedClient::instance()->getContext().getNetInterface();
		ScorchedClient::instance()->getContext().setNetInterface(0);
	}

	// Create the new net handlers
	if (ClientParams::instance()->getConnectedToServer())
	{
		ScorchedClient::instance()->getContext().setNetInterface(new NetServerTCP3());
	}
	else
	{
		ScorchedClient::instance()->getContext().setNetInterface(new NetLoopBack(false));
	}
	ScorchedClient::instance()->getNetInterface().setMessageHandler(
		&ScorchedClient::instance()->getComsMessageHandler());
	ClientAdmin::instance();

	return true;
}

static bool initComsHandlers()
{
	// Setup the coms handlers
	ScorchedClient::instance()->getComsMessageHandler().setConnectionHandler(
		ClientMessageHandler::instance());

	ClientChannelManager::instance();
	ClientConnectionRejectHandler::instance();
	ClientLinesHandler::instance();
	ClientConnectionAcceptHandler::instance();
	ClientConnectionAuthHandler::instance();
	ClientInitializeModHandler::instance();
	ClientFileHandler::instance();
	ClientOperationHandler::instance();
	ClientGameStoppedHandler::instance();
	ClientStartGameHandler::instance();
	ClientLoadLevelHandler::instance();
	ClientAdminResultHandler::instance();

	return true;
}

static bool initWindows(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp(LANG_RESOURCE("INITIALIZING_WINDOWS", "Initializing Windows"));
	if (!GLWWindowSkinManager::defaultinstance()->loadWindows())
	{
		S3D::dialogMessage("Scorched3D", "Failed to load default windows skins");
		return false;
	}
	ClientWindowSetup::setupStartWindows(GLWWindowSkinManager::defaultinstance());
	HelpButtonDialog::instance();

	Console::instance()->init();
	std::string errorString;
	if (!ConsoleFileReader::loadFileIntoConsole(S3D::getDataFile("data/autoexec.xml"), errorString))
	{
		S3D::dialogMessage("Failed to parse data/autoexec.xml", errorString);
		return false;
	}

	return true;
}

static bool initClient()
{
	ProgressCounter progressCounter;
	progressCounter.setUser(ProgressDialogSync::events_instance());
	progressCounter.setNewPercentage(0.0f);

	// Load in all the coms
	if (!initComs(&progressCounter)) return false;
	
	// Start the server (if required)
	if (!ClientParams::instance()->getConnectedToServer())
	{
		ScorchedServerSettings *settings = 0;

		if (ClientParams::instance()->getSaveFile()[0])
		{
			// Load the saved game state (settings)
			settings = new ScorchedServerSettingsSave(ClientParams::instance()->getSaveFile());
		}
		else
		{
			std::string clientFile = ClientParams::instance()->getClientFile();
			if (ClientParams::instance()->getStartCustom())
			{
				clientFile = S3D::getSettingsFile("singlecustom.xml");
			}

			// If not load the client settings file
			if (!S3D::fileExists(clientFile.c_str()))
			{
				S3D::dialogExit(scorched3dAppName, S3D::formatStringBuffer(
					"Client file \"%s\" does not exist.",
					clientFile.c_str()));
			}

			settings = new ScorchedServerSettingsOptions(clientFile,
				ClientParams::instance()->getRewriteOptions(),
				ClientParams::instance()->getWriteFullOptions());
		}

		if (!ScorchedServer::instance()->startServer(
			*settings,
			true,
			&progressCounter)) return false;
		delete settings;
	}

	ConnectDialog::instance()->start();
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimConnect);

	return true;
}

static bool startClientInternal()
{
	// Check if we are connecting to a server
	if (ClientParams::instance()->getConnect()[0])
	{
		return initClient();	
	}
	else if (ClientParams::instance()->getStartCustom() ||
		ClientParams::instance()->getClientFile()[0])
	{
		return initClient();
	}
	else if (ClientParams::instance()->getSaveFile()[0])
	{
		// Or the client saved game
		if (!S3D::fileExists(ClientParams::instance()->getSaveFile()))
		{
			S3D::dialogExit(scorched3dAppName, S3D::formatStringBuffer(
				"Client read saved game file \"%s\" does not exist.",
				ClientParams::instance()->getSaveFile()),
				false);
		}

		return initClient();
	}
	else
	{
		// Do nothing
	}

	return true;
}

bool ClientMain::startClient()
{
	AnimatedBackdropDialog::instance()->drawBackground();
	BackdropDialog::instance()->capture();

	return startClientInternal();
}

bool ClientMain::clientEventLoop(float frameTime)
{
	static SDL_Event event;
	bool idle = true;
	if (SDL_PollEvent(&event))
	{
		idle = false;
		switch (event.type)
		{
		/* keyboard events */
		case SDL_KEYUP:
			break;
		case SDL_KEYDOWN:
			/* keyevents are handled in mainloop */
			Keyboard::instance()->processKeyboardEvent(event);
			break;

			/* mouse events */
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			Mouse::instance()->processMouseEvent(event);
			break;
		case SDL_ACTIVEEVENT:
			if (event.active.gain == 0)
			{
				Gamma::instance()->reset();
			}
			else
			{
				Gamma::instance()->set();
			}

			paused = ( OptionsDisplay::instance()->getFocusPause() && (event.active.gain == 0));
			break;
		case SDL_VIDEORESIZE:
			/*Display::instance()->changeSettings(
				event.resize.w,event.resize.h, 
				OptionsDisplay::instance()->getFullScreen());*/
			MainCamera::instance()->getCamera().setWindowSize(
				event.resize.w, event.resize.h);
			Main2DCamera::instance()->getViewPort().setWindowSize(
				event.resize.w, event.resize.h);
				
			break;
		case SDL_QUIT:
			ScorchedClient::instance()->getMainLoop().exitLoop();
			break;
		}
	}

	return idle;
}

bool ClientMain::clientMain()
{
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	// Create the actual window
	if (!createScorchedWindow()) return false;

	// Start the initial windows
	ClientState::setupGameState();
	ProgressCounter progressCounter;
	ProgressDialog::instance()->changeTip();
	progressCounter.setUser(ProgressDialogSync::events_instance());
	progressCounter.setNewPercentage(0.0f);
	if (!initHardware(&progressCounter)) return false;
	if (!initWindows(&progressCounter)) return false;
	if (!initComsHandlers()) return false;

	OptionsDisplayConsole::instance()->addDisplayToConsole();

	// Try and start the client
	if (!startClientInternal()) return false;

	// Enter the SDL main loop to process SDL events
	Clock loopClock;
	for (;;)
	{
		float frameTime = loopClock.getTimeDifference();
		bool idle = clientEventLoop(frameTime);

		if (!ScorchedClient::instance()->getMainLoop().mainLoop()) break;
		if ((!paused) && (idle) )
		{
			ScorchedClient::instance()->getMainLoop().draw();
		}
		else
		{
			ClientProcessingLoop::instance()->dontLimitFrameTime();
		}
		if (paused) SDL_Delay(100);  // Otherwise when not drawing graphics its an infinite loop	
	}

	if (ScorchedClient::instance()->getContext().getNetInterfaceValid())
	{
		ScorchedClient::instance()->getNetInterface().disconnectAllClients();
	}
	GLWWindowManager::instance()->saveSettings();
    SDL_Delay(1000);
	Gamma::instance()->reset();
	Sound::instance()->destroy();
	Lang::instance()->saveUndefined();

	return true;
}

