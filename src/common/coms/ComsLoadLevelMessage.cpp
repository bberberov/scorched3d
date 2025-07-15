////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2024
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

#include <landscapedef/LandscapeDefinition.h>
#include <coms/ComsLoadLevelMessage.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <engine/Simulator.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankTeamScore.h>
#include <tankai/TankAIStore.h>
#include <weapons/AccessoryStore.h>
#include <server/ScorchedServer.h>
#include <set>
#ifndef S3D_SERVER
#include <tankgraph/TargetRendererImplTank.h>
#endif

ComsMessageType ComsLoadLevelMessage::ComsLoadLevelMessageType("ComsLoadLevelMessageType");

ComsLoadLevelMessage::ComsLoadLevelMessage() :
	ComsMessage(ComsLoadLevelMessageType),
	lastEmpty_(false)
{
}

ComsLoadLevelMessage::~ComsLoadLevelMessage()
{
}

bool ComsLoadLevelMessage::saveState(ScorchedContext &context)
{
	// State
	if (!context.getOptionsGame().getMainOptions().
		writeToBuffer(stateBuffer_, false, false)) return false;
	if (!context.getOptionsTransient().writeToBuffer(stateBuffer_)) return false;
	if (!context.getTankTeamScore().writeMessage(stateBuffer_)) return false;
	if (!context.getAccessoryStore().writeEconomyToBuffer(stateBuffer_)) return false;

	return false;
}

bool ComsLoadLevelMessage::saveTanks(ScorchedContext &context)
{
	// Tanks
	std::map<unsigned int, Tank *> &tanks =
		context.getTargetContainer().getTanks();
	tanksBuffer_.addToBuffer((int) tanks.size());
	std::map<unsigned int, Tank *>::iterator targetItor;
	for (targetItor = tanks.begin();
		targetItor != tanks.end();
		++targetItor)
	{
		Tank *tank = targetItor->second;
		tanksBuffer_.addToBuffer(tank->getPlayerId());
		if (!tank->writeMessage(tanksBuffer_)) return false;

		if (tank->getTankAI())
		{
			tanksBuffer_.addToBuffer(tank->getTankAI()->getName());
		}
		else
		{
			tanksBuffer_.addToBuffer("Human");
		}
	}

	return false;
}

bool ComsLoadLevelMessage::loadState(ScorchedContext &context, bool fullState)
{
	NetBufferReader reader(stateBuffer_);

	// State
	if (!context.getOptionsGame().getMainOptions().
		readFromBuffer(reader, false, false)) return false;
	if (fullState)
	{
		context.getOptionsGame().updateLevelOptions(
			context, landscapeDefinition_);
		if (!context.getOptionsTransient().readFromBuffer(reader)) return false;
		if (!context.getTankTeamScore().readMessage(reader)) return false;
		if (!context.getAccessoryStore().readEconomyFromBuffer(reader)) return false;
	}

	return true;
}

bool ComsLoadLevelMessage::loadTanks(ScorchedContext &context)
{
	/*
	 * FIXME
	 *
	 * There is a timing bug which shows on really fast machines
	 * where the client starts talking to the server before the
	 * server initial setup is done.
	 * Insofar as I have managed to debug this, the following
	 * happens on slower machines, aka the GOOD case:
	 *
	 * Server TankAddSimAction::invokeAction() new Tank "(Bot) Fred"
	 * Server TankAddSimAction::invokeAction() new Tank "(Bot) Ted"
	 * Server TankAddSimAction::invokeAction() new Tank "Player 1"
	 * Server TankAddSimAction::invokeAction() new Tank "Spectator"
	 * Client TankAddSimAction::invokeAction() new Tank "(Bot) Fred"
	 * Client TankAddSimAction::invokeAction() new Tank "(Bot) Ted"
	 * Client TankAddSimAction::invokeAction() new Tank "Player 1"
	 * Client TankAddSimAction::invokeAction() new Tank "Spectator"
	 *
	 * Note the server internally creates all tanks before the
	 * client does and thus before the client starts sending
	 * messages related to these tanks to the server.
	 *
	 * On a fast enough system (i7-10610U) the following order
	 * has been observed instead:
	 *
	 * Server TankAddSimAction::invokeAction() new Tank "(Bot) Fred"
	 * Server TankAddSimAction::invokeAction() new Tank "(Bot) Ted"
	 * Client TankAddSimAction::invokeAction() new Tank "(Bot) Fred"
	 * Client TankAddSimAction::invokeAction() new Tank "(Bot) Ted"
	 * Client TankAddSimAction::invokeAction() new Tank "Player 1"
	 * Client TankAddSimAction::invokeAction() new Tank "Spectator"
	 * Server TankAddSimAction::invokeAction() new Tank "Player 1"
	 * Server TankAddSimAction::invokeAction() new Tank "Spectator"
	 *
	 * Note the server creates the "Player 1" and "Spectator"
	 * tanks after the client, this causes the server to ignore
	 * some initial messages from the client related to these
	 * tanks, after which things get stuck, breaking non-networked
	 * games on fast machines.
	 *
	 * The sleep below is an ugly but effective workaround for this
	 * issue.
	 */
	SDL_Delay(100);

	NetBufferReader reader(tanksBuffer_);

	// Add any new tanks
	int targetCount = 0;
	if (!reader.getFromBuffer(targetCount)) return false;
	std::set<unsigned int> usedTargetIds;
	for (int i=0; i<targetCount; i++)
	{
		unsigned int playerId;
		if (!reader.getFromBuffer(playerId)) return false;
		usedTargetIds.insert(playerId);

		Tank *tank = context.getTargetContainer().getTankById(playerId);
		if (!tank)
		{
			tank = new Tank(
				context,
				playerId,
				0, 
				LangString(),
				Vector::getNullVector());
#ifndef S3D_SERVER
			if (!context.getServerMode())
			{
				tank->setRenderer(new TargetRendererImplTank(tank));
			}
#endif
			context.getTargetContainer().addTarget(tank);
		}

		if (!tank->readMessage(reader)) return false;

		std::string aiName;
		if (!reader.getFromBuffer(aiName)) return false;
		if (context.getServerMode() &&
			aiName != "Human")
		{
			TankAI *ai = ((ScorchedServer &)context).getTankAIs().
				getAIByName(aiName.c_str());
			if (!ai) return false;
			tank->setTankAI(ai->createCopy(tank));
		}
	}

	// Remove any tanks that have disconnected
	std::map<unsigned int, Tank *> tanks =
		context.getTargetContainer().getTanks(); // Copy
	std::map<unsigned int, Tank *>::iterator targetItor;
	for (targetItor = tanks.begin();
		targetItor != tanks.end();
		++targetItor)
	{
		Tank *tank = targetItor->second;
		if (usedTargetIds.find(tank->getPlayerId()) ==
			usedTargetIds.end())
		{
			Target *removedTank = 
				context.getTargetContainer().removeTarget(tank->getPlayerId());
			delete removedTank;
		}
	}

	return true;
}

void ComsLoadLevelMessage::addSimulation(ComsSimulateMessage &simulateMessage)
{
	if (lastEmpty_)
	{
		simulateBuffer_.setBufferUsed(emptyPosition_);
		lastEmpty_ = false;
	}

	if (simulateMessage.getActions().empty())
	{
		lastEmpty_ = true;
		emptyPosition_ = simulateBuffer_.getBufferUsed();
	}

	simulateMessage.writeMessage(simulateBuffer_);
}

bool ComsLoadLevelMessage::getSimulations(std::list<ComsSimulateMessage *> &simulateMessages)
{
	NetBufferReader reader(simulateBuffer_);
	while (reader.getBufferSize() - reader.getReadSize() > 0)
	{
		ComsSimulateMessage *message = new ComsSimulateMessage();
		if (!message->readMessage(reader)) return false;
		simulateMessages.push_back(message);
	}

	return true;
}

bool ComsLoadLevelMessage::writeMessage(NetBuffer &buffer)
{
	if (!landscapeDefinition_.writeMessage(buffer)) return false;
	buffer.addToBuffer(stateBuffer_);
	buffer.addToBuffer(actualTime_);
	buffer.addToBuffer(tanksBuffer_);
	buffer.addToBuffer(simulateBuffer_);

	return true;
}

bool ComsLoadLevelMessage::readMessage(NetBufferReader &reader)
{
	if (!landscapeDefinition_.readMessage(reader)) return false;
	if (!reader.getFromBuffer(stateBuffer_)) return false;
	if (!reader.getFromBuffer(actualTime_)) return false;
	if (!reader.getFromBuffer(tanksBuffer_)) return false;
	if (!reader.getFromBuffer(simulateBuffer_)) return false;

	return true;
}
