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

#include <dialogs/PlayerInGameDialog.h>
#include <target/TargetContainer.h>
#include <tanket/TanketType.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <tankai/TankAI.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <lang/LangResource.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>
#include <client/ClientChannelManager.h>
#include <GLW/GLWWindowManager.h>

PlayerInGameDialog *PlayerInGameDialog::instance()
{
	static PlayerInGameDialog instance_;
	return &instance_;
}

PlayerInGameDialog::PlayerInGameDialog() :
	PlayerDialog("Player", eSmallTitle, 
		"Allows the player to make changes to their\n"
		"name, their tank and to change teams.")
{
}

PlayerInGameDialog::~PlayerInGameDialog()
{
}

void PlayerInGameDialog::displayDialog()
{
	GLWWindowManager::instance()->showWindow(getId());
}

void PlayerInGameDialog::display()
{
	initialize();

	// Set cancel/spectate
	if (ClientParams::instance()->getConnectedToServer())
	{
		cancelButton_->setEnabled(true);
		spectateButton_->setEnabled(true);
	}
	else
	{
		cancelButton_->setEnabled(true);
		spectateButton_->setEnabled(false);
	}

	Tank *tank = getCurrentPlayer();
	if (!tank) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	initializeFromTank(tank);

	if (ClientParams::instance()->getConnectAcceptDefaults())
	{
		okButton(false);
	}
}

void PlayerInGameDialog::okButton(bool spectate)
{
	Tank *tank = getCurrentPlayer();
	if (!tank) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	// If we are connected online save this players name
	if (ClientParams::instance()->getConnectedToServer())
	{
		OptionsDisplay::instance()->getOnlineUserNameEntry().setValue(
			playerName_->getText().c_str());
		OptionsDisplay::instance()->getOnlineTankModelEntry().setValue(
			viewer_->getModelName());
		OptionsDisplay::instance()->getOnlineUserIconEntry().setValue(
			imageList_->getCurrentShortPath());
		if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
		{
			OptionsDisplay::instance()->getOnlineColorEntry().setValue(
				colorDropDown_->getCurrentColor());
		}
	}

	int currentTeam = ((ScorchedClient::instance()->getOptionsGame().getTeams() > 1)?
		teamDropDown_->getCurrentPosition() + 1:0);

	// Check the current model exists or get a random one
	TankModel *model = 
		ScorchedClient::instance()->getTankModels().
			getModelByName(viewer_->getModelName());
	if (!model)
	{
		model = ScorchedClient::instance()->getTankModels().
			getRandomModel(currentTeam, false, tankTypeDropDown_->getCurrentDataText());
	}

	// Get the player type
	const char *playerType = aiTypeDropDown_->getCurrentDataText();

	// Add this player
	ComsTankChangeMessage message(tank->getPlayerId(),
		playerName_->getLangString(),
		colorDropDown_->getCurrentColor(),
		tankTypeDropDown_->getCurrentDataText(),
		model->getName(),
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
		currentTeam,
		playerType,
		spectate);
	// Add avatar (if not one)
	if (tank && 
		strcmp(tank->getAvatar().getName(), imageList_->getCurrentShortPath()) != 0)
	{
		if (tank->getAvatar().loadFromFile(imageList_->getCurrentLongPath()))
		{
			if (tank->getAvatar().getFile().getBufferUsed() <=
				(unsigned) ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize())
			{
				message.setPlayerIconName(imageList_->getCurrentShortPath());
				message.getPlayerIcon().addDataToBuffer(
					tank->getAvatar().getFile().getBuffer(),
					tank->getAvatar().getFile().getBufferUsed());
			}
			else
			{
				ChannelText text("general", 
					LANG_RESOURCE_2("AVATAR_TOO_LARGE", 
					"Warning: Avatar too large to send to server, is {0} should be < {1}",
					tank->getAvatar().getFile().getBufferUsed(),
					ScorchedClient::instance()->getOptionsGame().getMaxAvatarSize()));
				ClientChannelManager::instance()->showText(text);
				Logger::log( "Warning: Avatar too large to send to server");
			}
		}
	}
	
	ComsMessageSender::sendToServer(message);
	GLWWindowManager::instance()->hideWindow(getId());
}

void PlayerInGameDialog::cancelButton()
{
	GLWWindowManager::instance()->hideWindow(getId());
}

Tank *PlayerInGameDialog::getCurrentPlayer()
{
	Tank *result = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (result) return result;
	if (ClientParams::instance()->getConnectedToServer())
	{
		std::map<unsigned int, Tank *> &tanks =
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = itor->second;
			if (tank->getDestinationId() == 
				ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
			{
				return tank;
			}
		}
	}
	return 0;
}

void PlayerInGameDialog::initializeFirst()
{
	if (ClientParams::instance()->getConnectedToServer())
	{
		// If we are connected online then use the online name
		playerName_->setText(LANG_STRING(OptionsDisplay::instance()->getOnlineUserName()));
		viewer_->selectModelByName(OptionsDisplay::instance()->getOnlineTankModel());
		imageList_->setCurrentShortPath(OptionsDisplay::instance()->getOnlineUserIcon());
		if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
		{
			colorDropDown_->setCurrentColor(OptionsDisplay::instance()->getOnlineColor());
		}
	}
}

void PlayerInGameDialog::initializeFromTank(Tank *tank)
{
	// Add teams/colors
	if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
	{
		// Add colors
		colorDropDown_->clear();
		std::map<unsigned int, Tank *> tanks =
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::vector<Vector *> availableColors =
			TankColorGenerator::instance()->getAvailableColors(tanks, tank);
		std::vector<Vector *>::iterator itor;
		for (itor = availableColors.begin();
			itor != availableColors.end();
			++itor)
		{
			Vector &color = *(*itor);
			colorDropDown_->addColor(color);
		}

		// Set color from tank
		colorDropDown_->setCurrentColor(tank->getColor());		
		viewer_->setTeam(0);
	}
	else
	{
		// Set team from tank
		int team = tank->getTeam();
		if (team == 0)
		{
			team = 
				ScorchedClient::instance()->getOptionsTransient().getLeastUsedTeam(
					ScorchedClient::instance()->getTargetContainer());
		}
		teamDropDown_->setCurrentPosition(team - 1);
		viewer_->setTeam(team);
	}

	// Set the current tankai
	aiTypeDropDown_->setCurrentPosition(0);
	if (tank->getTankAI())
	{
		aiTypeDropDown_->setCurrentText(
			LANG_RESOURCE(tank->getTankAI()->getName(), tank->getTankAI()->getName()));
	}

	// Set the anme
	playerName_->setText(tank->getTargetName());

	// Set the current avatar
	if (tank->getAvatar().getName()[0])
	{
		imageList_->setCurrentShortPath(tank->getAvatar().getName());
	}

	// Tank types
	tankTypeDropDown_->setCurrentText(
		LANG_RESOURCE(tank->getTanketType()->getName(), tank->getTanketType()->getName()));
	viewer_->setTankType(tank->getTanketType()->getName());

	// Tank model
	const char *tankModelName = tank->getModelContainer().getTankModel()->getName();
	viewer_->selectModelByName(tankModelName);
}
