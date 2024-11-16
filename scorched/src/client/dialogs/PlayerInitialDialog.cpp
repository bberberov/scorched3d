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

#include <dialogs/PlayerInitialDialog.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <coms/ComsTankChangeMessage.h>
#include <coms/ComsMessageSender.h>
#include <client/ClientChannelManager.h>
#include <GLW/GLWWindowManager.h>

PlayerInitialDialog *PlayerInitialDialog::instance()
{
	static PlayerInitialDialog instance_;
	return &instance_;
}

PlayerInitialDialog::PlayerInitialDialog() :
	PlayerDialog("Player", eSmallTitle | eHideName, ""),
	currentPlayerId_(0), allocatedTeam_(0)
{
}

PlayerInitialDialog::~PlayerInitialDialog()
{
}

void PlayerInitialDialog::displayDialog()
{
	// Set cancel/spectate
	cancelButton_->setEnabled(false);
	spectateButton_->setEnabled(false);

	GLWWindowManager::instance()->showWindow(getId());

	initialize();
	allocatedTeam_ = 0;
	currentPlayerId_ = 0;
	nextPlayer();
}

void PlayerInitialDialog::okButton(bool spectate)
{
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
	ComsTankChangeMessage message(currentPlayerId_,
		playerName_->getLangString(),
		colorDropDown_->getCurrentColor(),
		tankTypeDropDown_->getCurrentDataText(),
		model->getName(),
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId(),
		currentTeam,
		playerType,
		spectate);
	// Add avatar (if not one)
	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(currentPlayerId_);
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

	nextPlayer();
}

void PlayerInitialDialog::cancelButton()
{

}

void PlayerInitialDialog::nextPlayer()
{
	allocatedTeam_ = 0;
	getNextPlayer();
	if (currentPlayerId_ == 0) 
	{
		GLWWindowManager::instance()->hideWindow(getId());
	}
}

void PlayerInitialDialog::draw()
{
	if (ScorchedClient::instance()->getOptionsGame().getTeams() != 1)
	{
		// Auto select the team with the least players
		unsigned int newTeam = 
			ScorchedClient::instance()->getOptionsTransient().getLeastUsedTeam(
			ScorchedClient::instance()->getTargetContainer());
		if (newTeam != allocatedTeam_)
		{
			teamDropDown_->setCurrentPosition(newTeam - 1);
			allocatedTeam_ = newTeam;
			viewer_->setTeam(newTeam);
		}
	}
	GLWWindow::draw();
}

void PlayerInitialDialog::getNextPlayer()
{
	unsigned int current = currentPlayerId_;
	currentPlayerId_ = 0;

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if ((tank->getDestinationId() == 
			ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId()) &&
			(tank->getPlayerId() != TargetID::SPEC_TANK_ID))
		{
			if (current == 0)
			{
				currentPlayerId_ = tank->getPlayerId();
				initializeFromTank(tank);
				break;
			}
			else if (tank->getPlayerId() == current) 
			{
				current = 0;
			}
		}
	}
}

void PlayerInitialDialog::initializeFromTank(Tank *tank)
{
	playerName_->setText(tank->getTargetName());

	if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
	{
		// Add colors
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
}
