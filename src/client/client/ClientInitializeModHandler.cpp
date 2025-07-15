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

#include <client/ClientInitializeModHandler.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <sprites/ExplosionTextures.h>
#include <server/ScorchedServer.h>
#include <weapons/AccessoryStore.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <dialogs/ProgressDialog.h>
#include <engine/ModFiles.h>
#include <graph/OptionsDisplay.h>
#include <coms/ComsInitializeModMessage.h>
#include <coms/ComsMessageSender.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <GLEXT/GLLenseFlare.h>
#include <lang/LangResource.h>
#include <tankai/TankAIStrings.h>
#include <tank/TankModelStore.h>
#include <tanket/TanketTypes.h>

ClientInitializeModHandler *ClientInitializeModHandler::instance_ = 0;

ClientInitializeModHandler *ClientInitializeModHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientInitializeModHandler;
	}
	return instance_;
}

ClientInitializeModHandler::ClientInitializeModHandler()
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsInitializeModMessage::ComsInitializeModMessageType,
		this);
}

ClientInitializeModHandler::~ClientInitializeModHandler()
{
}

bool ClientInitializeModHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsInitializeModMessage message;
	if (!message.readMessage(reader)) return false;

	if (!initialize()) return false;

	// Tell the server we are initialized
	ComsMessageSender::sendToServer(message);
	return true;
}

bool ClientInitializeModHandler::initialize()
{
	// Re-load the tank names/sayings
	ScorchedClient::instance()->getTankAIStrings().load();

	// Load the accessory files
	if (!ScorchedClient::instance()->getAccessoryStore().parseFile(
		ScorchedClient::instance()->getContext(),
		ProgressDialogSync::events_instance())) return false;

	if (!ScorchedClient::instance()->getTanketTypes().
		loadTanketTypes(ScorchedClient::instance()->getContext())) {

		S3D::dialogMessage("Scorched 3D", "Failed to load tank types");
		return false;
	}

	// Load tank models here
	// This is after mods are complete but before any tanks models are used
	if (!ScorchedClient::instance()->getTankModels().loadTankMeshes(
		ScorchedClient::instance()->getContext(), 
		OptionsDisplay::instance()->getTankDetail(),
		ProgressDialogSync::events_instance()))
	{
		S3D::dialogMessage("Scorched 3D", "Failed to load all tank models");
		return false;
	}

	// Load the landscape definitions
	if (!ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions()) return false;

	// Load textures
	if (!ExplosionTextures::instance()->createTextures(
		ProgressDialogSync::events_instance())) 
			S3D::dialogExit("Scorched3D", "Failed to load explosion textures");
	GLLenseFlare::instance()->init(
		ProgressDialogSync::events_instance());
		
	// Load game windows
	ProgressDialogSync::events_instance()->setNewPercentage(0.0f);
	ProgressDialogSync::events_instance()->setNewOp(LANG_RESOURCE("INITIALIZING_WINDOWS", "Initializing Game Windows"));
	if (!GLWWindowSkinManager::modinstance()->loadWindows())
	{
		S3D::dialogExit("Scorched3D", "Failed to load windows skins");
	}
	ClientWindowSetup::setupGameWindows(GLWWindowSkinManager::modinstance());
	GLWWindowManager::instance()->loadSettings();
	return true;
}
