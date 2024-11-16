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

#include <client/ClientLoadLevelHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientReloadAdaptor.h>
#include <client/ClientSimulator.h>
#include <client/ClientParams.h>
#include <sound/Sound.h>
#include <common/Clock.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/ChannelText.h>
#include <common/ChannelManager.h>
#include <engine/MainLoop.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/CameraDialog.h>
#include <dialogs/PlayerInitialDialog.h>
#include <dialogs/PlayerInGameDialog.h>
#include <coms/ComsLoadLevelMessage.h>
#include <coms/ComsLevelLoadedMessage.h>
#include <coms/ComsMessageSender.h>
#include <graph/OptionsDisplayConsole.h>
#include <graph/ParticleEngine.h>
#include <graph/MainCamera.h>
#include <graph/SpeedChange.h>
#include <tankgraph/RenderTracer.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/Landscape.h>
#include <lang/LangResource.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankCamera.h>
#include <target/TargetRenderer.h>
#include <net/NetInterface.h>
#include <image/ImageFactory.h>
#include <GLW/GLWWindowManager.h>

ClientLoadLevelHandler *ClientLoadLevelHandler::instance_ = 0;

ClientLoadLevelHandler *ClientLoadLevelHandler::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientLoadLevelHandler();
	}

	return instance_;
}

ClientLoadLevelHandler::ClientLoadLevelHandler() : initialLevel_(false)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsLoadLevelMessage::ComsLoadLevelMessageType,
		this);
}

ClientLoadLevelHandler::~ClientLoadLevelHandler()
{

}

bool ClientLoadLevelHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	Clock generateClock;
	bool result = actualProcessMessage(netMessage, messageType, reader);
	float generateTime = generateClock.getTimeDifference();
	int idleTime = ScorchedClient::instance()->getOptionsGame().getBuyingTime();

	if (idleTime > 0 && int(generateTime) > idleTime - 5)
	{
		LangString message = LANG_RESOURCE("LEVEL_TIMEOUT_WARNING",
			"Warning: Your PC is taking a long time to generate levels.\n"
			"This may cause you to be kicked by some servers.\n"
			"You can fix this by lowering your display settings");

		Logger::log(LangStringUtil::convertFromLang(message));
		ChannelText text("info", message);
		ChannelManager::showText(ScorchedClient::instance()->getContext(), text);
	} 
	else
	{
		Logger::log(S3D::formatStringBuffer(
			"Finished loading landscape %.2f seconds", generateTime));
	}

	return result;
}

bool ClientLoadLevelHandler::actualProcessMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Make sure simulator knows we are loading a level
	ScorchedClient::instance()->getClientSimulator().setLoadingLevel(true);

	// Move into the load level state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimLoadLevel);

	// Read the message
	ComsLoadLevelMessage message;
	if (!message.readMessage(reader)) return false;

	// Read the state from the message
	if (!message.loadState(ScorchedClient::instance()->getContext())) return false;
	if (!message.loadTanks(ScorchedClient::instance()->getContext())) return false;

	// make sure we can only see the correct settings
	OptionsDisplayConsole::instance()->addDisplayToConsole();

	// Display info
	Logger::log(S3D::formatStringBuffer("Loading landscape %s (Defn: %s, Tex: %s)",
		message.getLandscapeDefinition().getName(),
		message.getLandscapeDefinition().getDefn(),
		message.getLandscapeDefinition().getTex()));

	// Set the progress dialog nicities
	ProgressDialog::instance()->changeTip();
	LandscapeDefinitionsEntry *landscapeDefinition =
		ScorchedClient::instance()->getLandscapes().getLandscapeByName(
		message.getLandscapeDefinition().getName());
	if (landscapeDefinition)
	{
		Image image = ImageFactory::loadImage(S3D::eModLocation,
			S3D::formatStringBuffer("data/landscapes/%s", 
			landscapeDefinition->picture.c_str()));
		ProgressDialog::instance()->setIcon(image);
	}

	// Generate new landscape
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		message.getLandscapeDefinition(),
		ProgressDialogSync::noevents_instance());

	// Reset and initialize simulator and
	ScorchedClient::instance()->getClientSimulator().newLevel();

	// Calculate all the new landscape settings (graphics)
	Landscape::instance()->generate(ProgressDialogSync::noevents_instance());

	// Add all missed actions to the simulator
	ProgressDialogSync::noevents_instance()->setNewOp(
		LANG_RESOURCE("EXECUTING_EVENTS", "Executing Events"));
	std::list<ComsSimulateMessage *> simulateMessages;
	std::list<ComsSimulateMessage *>::iterator messageItor;
	if (!message.getSimulations(simulateMessages)) return false;
	int i=0;
	for (messageItor = simulateMessages.begin();
		messageItor != simulateMessages.end();
		++messageItor, i++)
	{
		ProgressDialogSync::noevents_instance()->
			setNewPercentage(100.0f * float(i) / float(simulateMessages.size()));

		ComsSimulateMessage *simMessage = *messageItor;
		ScorchedClient::instance()->getClientSimulator().
			addComsSimulateMessage(*simMessage);
		delete simMessage;
	}
	simulateMessages.clear();

	// Move into the next state
	if (strcmp(message.getLandscapeDefinition().getName(), "blank") == 0)
	{
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWaitNoLandscape);
	}
	else
	{
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	}

	// Sync the simulator
	Sound::instance()->setPlaySounds(false);
	ProgressDialogSync::noevents_instance()->setNewOp(
		LANG_RESOURCE("SYNCING_SIMULATOR", "Syncing Simulator"));
	Clock generateClock;
	fixed actualTime = message.getActualTime();
	fixed actualTimeCurrent = 0;
	for (;;) 
	{
		actualTimeCurrent += fixed(1);
		if (actualTimeCurrent >= actualTime) 
		{
			ScorchedClient::instance()->getClientSimulator().
				setSimulationTime(actualTime);
			break;
		}

		float percentage = 100.0f * 
			float(actualTimeCurrent.asInt()) / float(actualTime.asInt());
		ProgressDialogSync::noevents_instance()->setNewPercentage(percentage);
		ScorchedClient::instance()->getClientSimulator().
			setSimulationTime(actualTimeCurrent);
	}
	Sound::instance()->setPlaySounds(true);
	float deformTime = generateClock.getTimeDifference();
	Logger::log(S3D::formatStringBuffer("Landscape sync event time %.2f seconds", deformTime));

	ProgressDialogSync::noevents_instance()->setNewOp(
		LANG_RESOURCE("PROCESSING_MESSAGES", "Processing Messages"));

	// Make sure the landscape has been optimized
	Landscape::instance()->recalculateLandscape();
	Landscape::instance()->recalculateRoof();

	// Reset stuff
	ScorchedClient::instance()->
		getParticleEngine().killAll();
	MainCamera::instance()->getTarget().
		getPrecipitationEngine().killAll();
	CameraDialog::instance()->getCamera().
		getPrecipitationEngine().killAll();
	RenderTracer::instance()->newGame();
	SpeedChange::instance()->resetSpeed();

	// Process any outstanding coms messages
	ScorchedClient::instance()->getNetInterface().processMessages();

	// As we have not returned to the main loop for ages the
	// timer will have a lot of time in it
	// Get rid of this time so we don't screw things up
	ScorchedClient::instance()->getMainLoop().getTimer().getTimeDifference();

	// Reset camera positions for each tank
	bool playerTanks = false;
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = ScorchedClient::instance()->getTargetContainer().getTanks().begin();
		tankItor != ScorchedClient::instance()->getTargetContainer().getTanks().end();
		++tankItor)
	{
		Tank *current = (*tankItor).second;
		current->getRenderer()->moved();
		current->getCamera().setCameraType(1);
		if (current->getDestinationId() != 0 &&
			current->getPlayerId() != TargetID::SPEC_TANK_ID)
		{
			playerTanks = true;
		}
	}
	if (playerTanks)
	{
		MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamSpectator);
	}
	ClientReloadAdaptor::instance();

	// Make sure simulator knows we are not loading a level
	ScorchedClient::instance()->getClientSimulator().setLoadingLevel(false);

	// Show the player selection dialogs
	if (initialLevel_)
	{
		initialLevel_ = false;
		if (ClientParams::instance()->getConnectedToServer())
		{
			PlayerInGameDialog::instance()->displayDialog();
			PlayerInGameDialog::instance()->initializeFirst();
		}
		else
		{
			PlayerInitialDialog::instance()->displayDialog();
		}
	}

	// Tell the server we have finished processing the landscape
	ComsLevelLoadedMessage levelLoadedMessage;
	ComsMessageSender::sendToServer(levelLoadedMessage);

	return true;
}
