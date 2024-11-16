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

#include <dialogs/PlayerDialog.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIStrings.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelStore.h>
#include <tank/TankModelContainer.h>
#include <tanket/TanketTypes.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/ChannelText.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWTextButton.h>
#include <image/ImageFactory.h>
#include <stdio.h>

PlayerDialog::PlayerDialog(const std::string &name, 
		unsigned int states, 
		const std::string &description) : 
	GLWWindow(name, 10.0f, 10.0f, 740.0f, 480.0f, 
		states,
		description),
	cancelButton_(0), viewer_(0), spectateButton_(0)
{
	needCentered_ = true;

	// Add buttons
	okButton_ = (GLWTextButton *)  addWidget(new GLWTextButton(LANG_RESOURCE("PLAY", "Play"), 665, 10, 65, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX));
	okButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("PLAY", "Play"), 
		LANG_RESOURCE("PLAY_PLAYER_DIALOG_TOOLTIP",
		"Start or continue playing.\n"
		"Apply all displayed player settings.")));

	spectateButton_ = (GLWTextButton *) addWidget(new GLWTextButton(LANG_RESOURCE("SPECTATE", "Spectate"), 555, 10, 105, this, 
		GLWButton::ButtonFlagCenterX));
	spectateButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SPECTATE", "Spectate"), 
		LANG_RESOURCE("SPECTATE_PLAYER_DIALOG_TOOLTIP",
		"Change into specator mode.\n"
		"This action will kill the player if they are alive.")));

	cancelButton_ =  (GLWTextButton *) addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 445, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX));
	cancelButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CANCEL", "Cancel"), 
		LANG_RESOURCE("CANCEL_PLAYER_DIALOG_TOOPTIP",
		"Close dialog without making any changes to the player.")));

	GLWPanel *infoPanel = new GLWPanel(10.0f, 390.0f, 720.0f, 75.0f,
		false, true, true);
	addWidget(infoPanel);

	// Create a texture to display the player color selector
	Image map = ImageFactory::loadImage(S3D::eDataLocation, "data/images/white.bmp");
	colorTexture_.create(map);

	// Create players avatar choice
	{
		infoPanel->addWidget(new GLWLabel(10, 25, LANG_RESOURCE("AVATAR_LABEL", "Avatar:")));
		avatarTip1_.setText(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("AVATAR", "Avatar"), 
			LANG_RESOURCE("AVATAR_TOOLTIP_CHANGE",
			"The current player's avatar.\n"
			"Click to change.\n"));
		imageList_ = new GLWImageList(95.0f, 20.0f);
		imageList_->addDirectory(S3D::getSettingsFile("avatars"));
		imageList_->addDirectory(S3D::getDataFile("data/avatars"));
		imageList_->setCurrentShortPath("player.png");
		imageList_->setToolTip(&avatarTip1_);
		imageList_->setName("Avatar");
		infoPanel->addWidget(imageList_);
	}

	// Create player name choice
	{
		ToolTip *nameTip = new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("PLAYER_NAME", "Player Name"),
			LANG_RESOURCE("PLAYER_NAME_TOOLTIP",
			"The name of this player.\n"
			"Use the backspace or delete key to remove this name.\n"
			"Type in a new player name via the keyboad to change."));
		GLWLabel *nameLabel = (GLWLabel *) 
			infoPanel->addWidget(new GLWLabel(145, 40, LANG_RESOURCE("NAME_LABEL", "Name:")));
		nameLabel->setToolTip(nameTip);
		playerName_ = (GLWTextBox *) 
			infoPanel->addWidget(new GLWTextBox(215, 40, 495, LANG_RESOURCE("PLAYER", "Player")));
		playerName_->setMaxTextLen(22);
		playerName_->setToolTip(nameTip);
		playerName_->setName("Name");
		playerName_->setAllowUnicode(
			ScorchedClient::instance()->getOptionsGame().getAllowMultiLingualNames());
	}
	
	// Create team choice
	{
		ToolTip *teamTip = new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("TEAM_SELECTION", "Team Selection"),
			LANG_RESOURCE("TEAM_SELECTION_TOOLTIP", "Change the team this player will join.\n"
			"This is only available when playing team games."));
		teamLabel_ = (GLWLabel *) 
			infoPanel->addWidget(new GLWLabel(145, 8, LANG_RESOURCE("TEAM_LABEL", "Team:")));
		teamLabel_->setToolTip(teamTip);
		teamDropDown_ = (GLWDropDownText *) 
			infoPanel->addWidget(new GLWDropDownText(215, 8, 120));
		teamDropDown_->setHandler(this);
		teamDropDown_->setToolTip(teamTip);
		teamDropDown_->setName("Team");
	}

	// Create color choice
	{
		ToolTip *colorTip = new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("COLOR_SELECTION", "Color Selection"),
			LANG_RESOURCE("COLOR_SELECTION_TOOLTIP", "Change the color this player displayed as.\n"
			"This is only available when playing non-team games."));
		colorLabel_ = (GLWLabel *) 
			infoPanel->addWidget(new GLWLabel(145, 8, LANG_RESOURCE("COLOR_LABEL", "Color:")));
		colorLabel_->setToolTip(colorTip);
		colorDropDown_ = (GLWDropDownColor *) 
			infoPanel->addWidget(new GLWDropDownColor(215, 8, 120));
		colorDropDown_->setHandler(this);
		colorDropDown_->setToolTip(colorTip);
		colorDropDown_->setName("Color");
	}

	// Create computer type choice
	{
		ToolTip *aiTypeTip = new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("PLAYER_TYPE", "Player AI"),
			LANG_RESOURCE("PLAYER_TYPE_TOOLTIP", "Change between human and computer controlled\n"
			"players.  This is only available when playing\n"
			"single player games."));
		GLWLabel *aiTypeLabel = (GLWLabel *) 
			infoPanel->addWidget(new GLWLabel(350, 8, LANG_RESOURCE("AI_LABEL", "AI:")));
		aiTypeLabel->setToolTip(aiTypeTip);
		aiTypeDropDown_ = (GLWDropDownText *) 
			infoPanel->addWidget(new GLWDropDownText(390, 8, 120));
		aiTypeDropDown_->setHandler(this);
		aiTypeDropDown_->setToolTip(aiTypeTip);
		aiTypeDropDown_->setName("Type");
	}

	// Create tank type choice
	{
		ToolTip *tankTypeTip = new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("TANK_TYPE", "Tank Type"),
			LANG_RESOURCE("TANK_TYPE_TOOLTIP", "Change between tanks with different abilities.\n"
			"This is only available for selected mods."));
		GLWLabel *tankTypeLabel = (GLWLabel *) 
			infoPanel->addWidget(new GLWLabel(525, 8, LANG_RESOURCE("TANK_TYPE_LABEL", "Type:")));
		tankTypeLabel->setToolTip(tankTypeTip);
		tankTypeDropDown_ = (GLWDropDownText *) 
			infoPanel->addWidget(new GLWDropDownText(590, 8, 120));
		tankTypeDropDown_->setHandler(this);
		tankTypeDropDown_->setToolTip(tankTypeTip);
		tankTypeDropDown_->setName("TankType");
	}

	// Create tank vieer
	{
		GLWPanel *infoPanel = new GLWPanel(10.0f, 40.0f, 720.0f, 330.0f,
			false, true, true);
		viewer_ = new GLWTankViewer(5.0f, 5.0f, 4, 3);
		viewer_->setName("Tank");
		infoPanel->addWidget(viewer_);
		addWidget(infoPanel);
	}

	humanToolTip_.setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("HUMAN", "Human"), 
		LANG_RESOURCE("HUMAN_TOOLTIP", "A human controlled player."));
}

PlayerDialog::~PlayerDialog()
{
}

void PlayerDialog::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	GLWWindow::keyDown(buffer, keyState, history, hisCount, skipRest);
	skipRest = true;
}

void PlayerDialog::select(unsigned int id, const int pos, 
	GLWSelectorEntry value)
{
	if (id == teamDropDown_->getId())
	{
		viewer_->setTeam(teamDropDown_->getCurrentPosition() + 1);
	}
	else if (id == tankTypeDropDown_->getId())
	{
		viewer_->setTankType(tankTypeDropDown_->getCurrentDataText());
	}
}

void PlayerDialog::buttonDown(unsigned int id)
{
	if (cancelButton_ && id == cancelButton_->getId())
	{
		cancelButton();
	}
	else if (id == okButton_->getId() ||
		id == spectateButton_->getId())
	{
		if (!playerName_->getText().empty())
		{
			okButton(id == spectateButton_->getId());
		}
	}
}

void PlayerDialog::initialize()
{
	// Init viewer
	viewer_->init();

	// Add teams/colors
	if (ScorchedClient::instance()->getOptionsGame().getTeams() == 1)
	{
		teamDropDown_->addText(LANG_RESOURCE("NONE", "None"), "None");
		teamDropDown_->setVisible(false);
		teamLabel_->setVisible(false);
		colorDropDown_->setVisible(true);
		colorLabel_->setVisible(true);
	}
	else
	{
		// Create team entries
		teamDropDown_->clear();
		for (int i=1; i<=ScorchedClient::instance()->getOptionsGame().getTeams(); i++)
		{
			const char *name = TankColorGenerator::getTeamName(i);
			GLWSelectorEntry entry(LANG_RESOURCE(name, name), 0, false, &colorTexture_, 0);
			entry.getColor() = TankColorGenerator::getTeamColor(i);
			teamDropDown_->addEntry(entry);
		}	
		teamDropDown_->setVisible(true);
		teamLabel_->setVisible(true);
		colorDropDown_->setVisible(false);
		colorLabel_->setVisible(false);
	}

	// Add player types
	// Set TankAIs
	static TankAIStore tankAIStore;
	tankAIStore.clearAIs();
	tankAIStore.loadAIs(true);
	aiTypeDropDown_->clear();
	aiTypeDropDown_->addEntry(GLWSelectorEntry(LANG_RESOURCE("HUMAN", "Human"), 
		&humanToolTip_, false, 0, 0, "Human"));
	if (!ClientParams::instance()->getConnectedToServer() &&
		!ScorchedClient::instance()->getOptionsGame().getTutorial()[0])
	{
		std::list<TankAI *>::iterator aiitor;
		for (aiitor = tankAIStore.getAis().begin();
			aiitor != tankAIStore.getAis().end();
			++aiitor)
		{
			TankAI *ai = (*aiitor);
			if (ai->availableForPlayers())
			{
				aiTypeDropDown_->addEntry(
					GLWSelectorEntry(LANG_RESOURCE(ai->getName(), ai->getName()),
						(*aiitor)->getToolTip(), false, 0, 0, ai->getName()));
			}
		}
	}

	// Set the current avatar
	imageList_->setCurrentShortPath("player.png");

	// Tank types
	tankTypeDropDown_->clear();
	std::vector<TanketType *> &tanketTypes = ScorchedClient::instance()->getTanketTypes().getTypes();
	std::vector<TanketType *>::iterator tanketTypesItor;
	for (tanketTypesItor = tanketTypes.begin();
		tanketTypesItor != tanketTypes.end();
		++tanketTypesItor)
	{
		TanketType *type = *tanketTypesItor;
		tankTypeDropDown_->addEntry(
			GLWSelectorEntry(
				LANG_RESOURCE(type->getName(), type->getName()),
				type->getTooltip(), 
				false, 0, 0, type->getName()));
	}
}
