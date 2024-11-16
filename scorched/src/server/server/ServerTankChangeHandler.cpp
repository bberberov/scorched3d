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

#include <server/ServerTankChangeHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerChannelManager.h>
#include <server/ServerSimulator.h>
#include <server/ServerState.h>
#include <server/ServerTextFilter.h>
#include <server/ServerAuthHandler.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <coms/ComsTankChangeMessage.h>
#include <simactions/TankChangeSimAction.h>
#include <tankai/TankAIStore.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankModelStore.h>

ServerTankChangeHandler::ServerTankChangeHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsTankChangeMessage::ComsTankChangeMessageType,
		this);
}

ServerTankChangeHandler::~ServerTankChangeHandler()
{
}

bool ServerTankChangeHandler::processMessage(NetMessage &netMessage,
	const char *messageType, NetBufferReader &reader)
{
	ComsTankChangeMessage message;
	if (!message.readMessage(reader)) return false;

	// Validate player
	unsigned int playerId = message.getPlayerId();
	Tank *tank = ScorchedServer::instance()->getTargetContainer().getTankById(playerId);
	if (!tank) return true;

	// Add a computer player (if chosen and a single player match)
	if (0 != strcmp(message.getPlayerType(), "Human"))
	{
		if (ScorchedServer::instance()->getServerState().getState() !=
			ServerState::ServerWaitingForPlayersState)
		{
			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("info", "CHANGE_WHEN_STARTED", 
					"Can only change type before game starts."),
					netMessage.getDestinationId(),
					false);
			return true;
		}

		// Only allow this on a single player game
#ifdef S3D_SERVER
		return true;
#endif // #ifdef S3D_SERVER

		// Check tank ai is valid
		TankAI *ai = 
			ScorchedServer::instance()->getTankAIs().
			getAIByName(message.getPlayerType());
		if (!ai) return true;
	}

	// Setup the new player
	LangString name(message.getPlayerName());
	filterName(tank, name);
	message.setPlayerName(name);

	// Check model
	TankModel *model = ScorchedServer::instance()->getTankModels().getModelByName(message.getModelName());
	if (!model->isOfAi(tank->getDestinationId() == 0) ||
		!model->isOfTankType(message.getTankType()) ||
		!model->isOfTeam(message.getPlayerTeam()))
	{
		model = ScorchedServer::instance()->getTankModels().getRandomModel(
			message.getPlayerTeam(),
			tank->getDestinationId() == 0,
			message.getTankType());
		message.setModelName(model->getName());
	}

	// Send to client
	TankChangeSimAction *simAction = new TankChangeSimAction(message);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);

	return true;
}

bool ServerTankChangeHandler::filterName(Tank *tank,
	LangString &sentname)
{
	LangString originalname = sentname;

	// Remove spaces from the front and end of the name 
	LangStringUtil::trim(sentname);

	// Ensure this name does not have any "bad" words in it
	ScorchedServer::instance()->getTextFilter().filterString(sentname);

	// Remove unwanted characters from middle
	for (unsigned int *c = (unsigned int *) sentname.c_str(); *c;  c++)
	{
		if (*c == '\"') *c = '\'';
		else if (*c == ']') *c = ')';
		else if (*c == '[') *c = '(';
		else if (*c == '%') *c = '$'; // Save problems with special chars
		if (!ScorchedServer::instance()->getOptionsGame().getAllowMultiLingualNames())
		{
			if (*c > 127) *c = '?';
		}
	}

	// Ensure this name does not have the bot name in it
	LangString botPrefix = 
		LANG_STRING(ScorchedServer::instance()->getOptionsGame().getBotNamePrefix());
	unsigned int *botPrefixPos = LangStringUtil::stristr(sentname.c_str(), botPrefix);
	if (botPrefixPos)
	{
		for (int i=0; i<(int) botPrefix.size(); i++, botPrefixPos++)
		{
			(*botPrefixPos) = '*';
		}
	}		

	// Check the client provides a name with a least 1 char in it
	// and the name is less than 16 chars
	if (sentname.size() == 0) sentname = LANG_STRING("NoName");
	if (sentname.size() > 22) sentname = sentname.substr(0, 22);

	// Make sure that no-one else has the same registered name
	// except the prefered user that has this name
	if (ScorchedServer::instance()->getOptionsGame().getRegisteredUserNames())
	{
		ServerAuthHandler *authHandler =
			ScorchedServer::instance()->getAuthHandler();
		if (authHandler)
		{
			while (!authHandler->authenticateUserName(tank->getUniqueId(),
				sentname))
			{
				sentname += LANG_STRING("(2)");
			}
		}
	}

	return (sentname != originalname);
}
