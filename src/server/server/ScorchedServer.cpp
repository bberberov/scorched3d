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

#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <server/ServerDestinations.h>
#include <server/ServerAuthHandlerStore.h>
#include <server/ServerTimedMessage.h>
#include <server/ServerBanned.h>
#include <server/ServerTextFilter.h>
#include <server/ServerHandlers.h>
#include <server/ServerLoadLevel.h>
#include <server/ServerChannelManager.h>
#include <server/ServerAdminSessions.h>
#include <server/ServerSyncCheck.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerFileServer.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <engine/ModFiles.h>
#include <engine/ActionController.h>
#include <engine/SaveGame.h>
#include <events/EventHandlerDataBase.h>
#include <events/EventHandlerAchievementNumberRankKills.h>
#include <events/EventController.h>
#include <tank/TankDeadContainer.h>
#include <target/TargetContainer.h>
#include <tank/TankModelStore.h>
#include <tanket/TanketTypes.h>
#include <tankai/TankAIStrings.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIWeaponSets.h>
#include <tankai/TankAIAdder.h>
#include <target/TargetSpace.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <coms/ComsSimulateResultMessage.h>
#include <lua/LUAScriptHook.h>
#include <weapons/EconomyStore.h>
#include <weapons/AccessoryStore.h>
#include <net/NetServerTCP3.h>
#include <net/NetLoopBack.h>

#ifndef S3D_SERVER
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#endif

ScorchedServer *ScorchedServer::instance_ = 0;
static ScorchedServer *instanceLock = 0;
bool ScorchedServer::started_ = false;
TargetSpace *ScorchedServer::targetSpace_ = new TargetSpace();

ScorchedServer *ScorchedServer::instance()
{
	return instance_;
}

bool ScorchedServer::startServer(const ScorchedServerSettings &settings, 
	bool local, ProgressCounter *counter)
{
#ifndef S3D_SERVER
	if (ClientParams::instance()->getConnectedToServer() &&
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId() != 0)
	{
		DIALOG_ASSERT(0);
	}
#endif

	stopServer();

	DIALOG_ASSERT(!instanceLock);
	instanceLock = new ScorchedServer;
	instance_ = instanceLock;
	instanceLock = 0;

	started_ = instance_->startServerInternal(settings, local, counter);
	return started_;
}

void ScorchedServer::stopServer()
{
	if (instance_)
	{
		delete instance_;
		instance_ = 0;
	}
	started_ = false;
}

ScorchedServer::ScorchedServer() : 
	ScorchedContext("Server")
{
	targetSpace_->setContext(this);

	economyStore_ = new EconomyStore();
	serverState_ = new ServerState();
	serverFileServer_ = new ServerFileServer();
	serverSimulator_ = new ServerSimulator();
	serverSimulator_->setScorchedContext(this);
	serverDestinations_ = new ServerDestinations();
	deadContainer_ = new TankDeadContainer();
	tankAIStore_ = new TankAIStore();
	authHandler_ = new ServerAuthHandlerStore();
	timedMessage_ = new ServerTimedMessage();
	bannedPlayers_ = new ServerBanned();
	textFilter_ = new ServerTextFilter();
	serverLoadLevel_ = new ServerLoadLevel(getComsMessageHandler());
	serverChannelManager_ = new ServerChannelManager(getComsMessageHandler());
	serverAdminSessions_ = new ServerAdminSessions();
	serverSyncCheck_ = new ServerSyncCheck(getComsMessageHandler());
	serverHandlers_ = new ServerHandlers(getComsMessageHandler());
	getComsMessageHandler().addHandler(
		ComsSimulateResultMessage::ComsSimulateResultMessageType,
		serverSimulator_);
	getLUAScriptHook().addHookProvider("server_channeltext");

	serverMessageHandler_ = new ServerMessageHandler();
	getComsMessageHandler().setConnectionHandler(serverMessageHandler_);
}

ScorchedServer::~ScorchedServer()
{
	targetSpace_->clear();
	delete deadContainer_;
	delete tankAIStore_;
	delete serverSimulator_;
	delete serverDestinations_;
	delete serverState_;
	delete authHandler_;
	delete timedMessage_;
	delete bannedPlayers_;
	delete textFilter_;
	delete serverHandlers_;
	delete serverLoadLevel_;
	delete serverChannelManager_;	
	delete serverAdminSessions_;
	delete serverSyncCheck_;
	delete serverMessageHandler_;
	delete serverFileServer_;
	delete economyStore_;
}

Simulator &ScorchedServer::getSimulator() 
{ 
	return *serverSimulator_; 
}

ServerAuthHandler *ScorchedServer::getAuthHandler()
{
	return authHandler_->getAuthHandler();
}

ServerConnectAuthHandler &ScorchedServer::getServerConnectAuthHandler()
{
	return serverHandlers_->getServerConnectAuthHandler();
}

bool ScorchedServer::startServerInternal(const ScorchedServerSettings &settings, 
	bool local, ProgressCounter *counter)
{
	Logger::log(S3D::formatStringBuffer("Scorched3D - Version %s (%s) - %s",
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str()));

	std::string settingsType = ((ScorchedServerSettings &) settings).type();
	if (settingsType == "FILE")
	{
		ScorchedServerSettingsOptions &options = (ScorchedServerSettingsOptions &) settings;

		// Load options
		getOptionsGame().getMainOptions().readOptionsFromFile(options.settingsFile_);
		if (options.rewriteOptions_)
		{
			getOptionsGame().getMainOptions().writeOptionsToFile(
				options.settingsFile_,
				options.writeFullOptions_);
		}
	}
	else if (settingsType == "SAVE")
	{
		ScorchedServerSettingsSave &options = (ScorchedServerSettingsSave &) settings;

		// Load the saved game state (settings)
		if (!SaveGame::loadState(options.saveFile_))
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Cannot load save file \"%s\".",
				options.saveFile_.c_str()), 
				false);			
		}
	}
	else
	{
		S3D::dialogExit("Scorched3D", "ScorchedServer::startServerInternal - Unknown settings type");
	}

	// Setup the message handling classes
	if (local)
	{
		setNetInterface(new NetLoopBack(true));
	}
	else
	{
		// Only create a net server for the actual multiplayer case
		// A loopback is created by the client for a single player game 
		setNetInterface(new NetServerTCP3());
	}

	getOptionsGame().updateChangeSet();
	getNetInterface().setMessageHandler(&getComsMessageHandler());

	// Set the mod
	S3D::setDataFileMod(getOptionsGame().getMod());

	// Load mod
#ifdef S3D_SERVER
	{
		if (!getModFiles().loadModFiles(getOptionsGame().getMod(), false, counter)) return false;
	}
#endif
	
	// Parse config
	if (!getAccessoryStore().parseFile(getContext(), counter)) return false;
	if (!getTanketTypes().loadTanketTypes(getContext())) return false;
	if (!getTankModels().loadTankMeshes(getContext(), 2, counter)) return false;
	getOptionsTransient().reset();
	if (!getLandscapes().readLandscapeDefinitions()) return false;

	// Add the server side bots
	// Add any new AIs
	if (!getTankAIs().loadAIs()) return false;
	TankAIAdder::addTankAIs(*this);
	getTankAIStrings().load();

	checkSettings();

	// Load all script hooks
	if (!getLUAScriptHook().loadHooks()) return false;

	// Add event hooks
	eventHandlerDataBase_ = EventHandlerDataBase::createInstance();
	if (eventHandlerDataBase_)
	{
		getEventController().addEventHandler(eventHandlerDataBase_);
	}
	getEventController().addEventHandler(new EventHandlerAchievementNumberRankKills(eventHandlerDataBase_));

#ifndef S3D_SERVER
	new ConsoleRuleMethodIAdapter<ActionController>(
		&getActionController(), 
		&ActionController::logActions, "ActionsLog");
	new ConsoleRuleMethodIAdapter<ActionController>(
		&getActionController(), 
		&ActionController::startActionProfiling, "ActionsProfilingStart");
	new ConsoleRuleMethodIAdapter<ActionController>(
		&getActionController(), 
		&ActionController::stopActionProfiling, "ActionsProfilingStop");
#endif

	return true;
}

void ScorchedServer::checkSettings()
{
	getLandscapes().checkEnabled(getOptionsGame());
	
	if (getOptionsGame().getTeamBallance() == OptionsGame::TeamBallanceBotsVs &&
		getOptionsGame().getTeams() > 2)
	{
		S3D::dialogExit("ScorchedServer",
			"Cannot start a game with more than 2 teams in the bots vs mode");		
	}
}
