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

#include <simactions/TankAddSimAction.h>
#include <simactions/TankChangeSimAction.h>
#include <simactions/TankRankSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelContainer.h>
#include <tanket/TanketType.h>
#include <tankai/TankAIStore.h>
#include <common/ChannelManager.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <events/EventController.h>
#include <events/EventHandlerDataBase.h>
#include <coms/ComsTankChangeMessage.h>
#include <server/ServerBanned.h>
#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerDestinations.h>
#include <server/ServerMessageHandler.h>
#ifndef S3D_SERVER
#include <client/ClientChannelManager.h>
#include <tankgraph/TargetRendererImplTank.h>
#endif

unsigned int TankAddSimAction::TankAddSimActionCount = 0;

REGISTER_CLASS_SOURCE(TankAddSimAction);

TankAddSimAction::TankAddSimAction()
{
	TankAddSimActionCount++;
}

TankAddSimAction::TankAddSimAction(
	unsigned int playerId, unsigned int destinationId,
	const std::string &uniqueId, const std::string &sUID, const std::string &hostDesc,
	unsigned int ipAddress, 
	const LangString &playerName, const std::string &aiName) :
	playerId_(playerId), destinationId_(destinationId),
	uniqueId_(uniqueId), sUID_(sUID), hostDesc_(hostDesc),
	ipAddress_(ipAddress), aiName_(aiName), playerName_(playerName)
{
	TankAddSimActionCount++;
}

TankAddSimAction::~TankAddSimAction()
{
	TankAddSimActionCount--;
}

bool TankAddSimAction::invokeAction(ScorchedContext &context)
{
	// Alocate new color
	Vector color = TankColorGenerator::instance()->getNextColor(
		context.getTargetContainer().getTanks());
	if (playerId_ == TargetID::SPEC_TANK_ID)
	{
		color = Vector(0.7f, 0.7f, 0.7f);
	}

	// Create tank
	Tank *tank = new Tank(
		context,
		playerId_,
		destinationId_,
		playerName_,
		color);

	// Choose team
	int team = 0;
	if (context.getOptionsGame().getTeams() > 1)
	{
		if (destinationId_ == 0 && 
			context.getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceBotsVs)
		{
			team = 1;
		}
		else
		{
			team = context.getOptionsTransient().getLeastUsedTeam(
				context.getTargetContainer());
		}
	}
	tank->setTeam(team);

	// Setup server stuff
	if (context.getServerMode())
	{
		tank->setUniqueId(uniqueId_.c_str());
		tank->setSUI(sUID_.c_str());
		tank->setHostDesc(hostDesc_.c_str());
		tank->setIpAddress(ipAddress_);
		if (aiName_.c_str()[0])
		{
			TankAI *ai = ((ScorchedServer &)context).getTankAIs().
				getAIByName(aiName_.c_str());
			tank->setTankAI(ai->createCopy(tank));
		}
	}

	// Add this tank 
	context.getTargetContainer().addTarget(tank);
	context.getEventController().tankConnected(tank);

	if (context.getServerMode())
	{
		if (((ScorchedServer &)context).getEventHandlerDataBase())
		{
			EventHandlerDataBase::TankRank rank = ((ScorchedServer &)context).
				getEventHandlerDataBase()->tankRank(tank);
			TankRankSimAction *rankSimAction = new TankRankSimAction();
			rankSimAction->addRank(rank);
			((ScorchedServer &)context).getServerSimulator().addSimulatorAction(rankSimAction);
		}

		if (destinationId_ == 0)
		{
			// Form the tank ai model
			TankModel *tankModel = 
				context.getTankModels().getRandomModel(team, true, tank->getTanketType()->getName());
			TankAvatar tankAvatar;
			tankAvatar.loadFromFile(S3D::getDataFile("data/avatars/computer.png"));

			// Tell the clients how to create this tank
			ComsTankChangeMessage tankChangeMessage(
				playerId_,
				playerName_,
				color,
				tank->getTanketType()->getName(),
				tankModel->getName(),
				0,
				team,
				"",
				false); 
			tankChangeMessage.setPlayerIconName("data/avatars/computer.png");
			tankChangeMessage.getPlayerIcon().addDataToBuffer(
				tankAvatar.getFile().getBuffer(),
				tankAvatar.getFile().getBufferUsed());

			TankChangeSimAction *changeAction = new TankChangeSimAction(tankChangeMessage);
			((ScorchedServer &)context).getServerSimulator().addSimulatorAction(changeAction);
		}
	}
	if (destinationId_ == 0)
	{
		tank->getState().setState(TankState::sSpectator);
	}

	if (scoreNetBuffer_.getBufferUsed() > 0)
	{
		NetBufferReader reader(scoreNetBuffer_);
		if (!tank->getAccessories().readMessage(reader) ||
			!tank->getScore().readMessage(reader))
		{
			Logger::log("ERROR: TankAddSimAction failed to update residual player info (read)");
		}
	}

#ifndef S3D_SERVER
	if (!context.getServerMode())
	{
		tank->setRenderer(new TargetRendererImplTank(tank));
		if (ClientChannelManager::instance()->getMutedPlayers().find(tank->getPlayerId())
			!= ClientChannelManager::instance()->getMutedPlayers().end())
		{
			tank->getState().setMuted(true);
		}
	}
#else
	if (context.getServerMode())
	{
		// Add to dialog
		Logger::log(S3D::formatStringBuffer("Player connected dest=\"%i\" id=\"%i\" name=\"%s\" unique=[%s] SUI=[%s]",
			tank->getDestinationId(),
			tank->getPlayerId(),
			tank->getCStrName().c_str(),
			tank->getUniqueId(),
			tank->getSUI()));
	}
	// Check if admin muted
	if (ipAddress_ != 0)
	{
		ServerBanned::BannedType type = 
			ScorchedServer::instance()->getBannedPlayers().getBanned(tank->getUniqueId(), tank->getSUI());
		if (type == ServerBanned::Muted)	
		{
			tank->getState().setMuted(true);
			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("admin", 
					"PLAYER_ADMIN_MUTED", 
					"Player admin muted [p:{0}]",
					tank->getTargetName()),
					true);
		}
		else if (type == ServerBanned::Flagged)
		{
			ScorchedServer::instance()->getServerChannelManager().sendText( 
				ChannelText("admin",
					"PLAYER_ADMIN_FLAGGED",
					"Player admin flagged [p:{0}]",
					tank->getTargetName()),
					true);
		}
	}
#endif

	if (context.getServerMode() && destinationId_)
	{	
		// Check the server destination actualy exists for this tank
		// It may not if the destination was closed during tank creation
		ServerDestination *serverDestination = 
			ScorchedServer::instance()->getServerDestinations().getDestination(destinationId_);
		if (!serverDestination)
		{
			ScorchedServer::instance()->getServerMessageHandler().destroyPlayer(playerId_, "Connection closed");
		}
	}

	if (context.getOptionsGame().getActionSyncCheck()) 
	{
		context.getSimulator().
			addSyncCheck(S3D::formatStringBuffer("TankAdd : %u:%u %s %s", 
				tank->getDestinationId(), tank->getPlayerId(),  
				tank->getCStrName().c_str(), 
				tank->getState().getSmallStateString()));
	}

	ChannelText text("info", 
		"PLAYER_CONNECTED",
		"Player connected [p:{0}]",
		tank->getTargetName());
	ChannelManager::showText(context, text);

	return true;
}

bool TankAddSimAction::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(playerName_);
	buffer.addToBuffer(scoreNetBuffer_);

	return true;
}

bool TankAddSimAction::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(destinationId_)) return false;
	if (!reader.getFromBuffer(playerName_)) return false;
	if (!reader.getFromBuffer(scoreNetBuffer_)) return false;
	
	return true;
}
