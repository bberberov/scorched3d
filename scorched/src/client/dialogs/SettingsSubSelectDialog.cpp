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

#include <dialogs/SettingsSubSelectDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <landscapedef/LandscapeDefinitionsBase.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWOptionEntry.h>
#include <GLW/GLWTabContainer.h>
#include <GLW/GLWSpacer.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLTextureStore.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>

SettingsSubSelectDialogListItem::SettingsSubSelectDialogListItem(
	const char *icon, const char *name, const char *description,
	bool selected) :
	name_(name), 
	selected_(0.0f, 0.0f, selected), 
	tip_(ToolTip::ToolTipHelp, LANG_STRING(name), LANG_STRING(description)),
	icon_(0.0f, 0.0f, 40.0f, 40.0f)
{
	if (S3D::fileExists(icon))
	{
		icon_.setTextureImage(ImageID(S3D::eAbsLocation, icon));
	}
}

SettingsSubSelectDialogListItem::~SettingsSubSelectDialogListItem()
{
}

void SettingsSubSelectDialogListItem::draw(float x, float y, float w)
{
	icon_.setX(x + 2.0f);
	icon_.setY(y + 2.0f);
	icon_.draw();

	GLWToolTip::instance()->addToolTip(&tip_, 
		GLWTranslate::getPosX() + x, 
		GLWTranslate::getPosY() + y, w, 50.0f);

	GLWFont::instance()->getGameFont()->drawWidth(
		w - 50.0f,
		GLWFont::widgetFontColor, 
		12.0f, x + 75.0f, y + 17.0f, 0.0f, 
		name_);

	selected_.setX(x + 50.0f);
	selected_.setY(y + 13.0f);
	selected_.draw();
}

SettingsSubSelectDialog *SettingsSubSelectDialog::instance_ = 0;

SettingsSubSelectDialog *SettingsSubSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SettingsSubSelectDialog;
	}
	return instance_;
}

SettingsSubSelectDialog::SettingsSubSelectDialog() : 
	GLWWindow("SettingsSubSelect", 700.0f, 540.0f, eHideName, "")
{
	// All the tabs are auto-layed out using a layout manager
	// except for the land tab where a panel on the land tab is layed out
	// and the rest are placed manually
	//
	// Create Tabs
	GLWTabContainer *tabPanel = new GLWTabContainer(0.0f, 0.0f, 0.0f, 0.0f);
	mainTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Main", LANG_RESOURCE("MAIN_TAB", "Main"), 10, 40, 520, 315));
	mainTab_->setGridWidth(2);
	mainTab_->setLayout(GLWPanel::LayoutGrid);
	moneyTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Money", LANG_RESOURCE("ECONOMY_TAB", "Money"), 10, 40, 520, 315));
	moneyTab_->setGridWidth(2);
	moneyTab_->setLayout(GLWPanel::LayoutGrid);
	scoreTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Score", LANG_RESOURCE("SCORE_TAB", "Score"), 10, 40, 520, 315));
	scoreTab_->setGridWidth(2);
	scoreTab_->setLayout(GLWPanel::LayoutGrid);
	envTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Env", LANG_RESOURCE("ENVIRONMENT_TAB", "Env"), 10, 40, 520, 315));
	envTab_->setGridWidth(2);
	envTab_->setLayout(GLWPanel::LayoutGrid);
	weaponsTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Weapon", LANG_RESOURCE("WEAPON_TAB", "Weapon"), 10, 40, 520, 315));
	weaponsTab_->setGridWidth(2);
	weaponsTab_->setLayout(GLWPanel::LayoutGrid);
	landTab_ = (GLWTab *)
		tabPanel->addWidget(new GLWTab("Land", LANG_RESOURCE("LANDSCAPE_TAB", "Land"), 10, 40, 520, 315));

	GLWPanel *landSettingsPanel = new GLWPanel(10.0f, 10.0f, 200.0f, 40.0f, false, false);
	landTab_->addWidget(landSettingsPanel);
	landSettingsPanel->setGridWidth(2);
	landSettingsPanel->setLayout(GLWPanel::LayoutGrid);
	addWidget(tabPanel, 0, SpaceAll, 10.0f);

	OptionsGame &optionsGame = SettingsSelectDialog::instance()->getOptions();

	// Add Options
	// Main Tab
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getTeamBallanceEntry());
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getNoRoundsEntry());
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getNoTurnsEntry());
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getStartTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getShotTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getRemoveTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getAIShotTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getBuyingTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getRoundTimeEntry());	
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getResignModeEntry());
	GLWOptionEntry::createEntry(
		controls_, mainTab_, optionsGame.getPlayerLivesEntry());
	mainTab_->addWidget(new GLWSpacer(0.0f, 20.0f));

	// Eco Tab
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getBuyOnRoundEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getEconomyEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getStartMoneyEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getInterestEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyPerRoundEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonPerAssistPointEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonPerKillPointEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonPerMultiKillPointEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonPerHitPointEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonForRoundEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyWonForLivesEntry());
	GLWOptionEntry::createEntry(
		controls_, moneyTab_, optionsGame.getMoneyPerHealthPointEntry());
	moneyTab_->addWidget(new GLWSpacer(0.0f, 10.0f));

	// Score tab
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScoreWonForRoundEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScoreWonForLivesEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScorePerKillEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScorePerAssistEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScorePerMoneyEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScorePerResignEntry());
	GLWOptionEntry::createEntry(
		controls_, scoreTab_, optionsGame.getScorePerSpectateEntry());
	scoreTab_->addWidget(new GLWSpacer(0.0f, 20.0f));

	// Env Tab
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getWindForceEntry());
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getWindTypeEntry());
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getWallTypeEntry());
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getMinFallingDistanceEntry());
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getMaxClimbingDistanceEntry());
	GLWOptionEntry::createEntry(
		controls_, envTab_, optionsGame.getTankFallingDamageEntry());
	envTab_->addWidget(new GLWSpacer(0.0f, 20.0f));

	// Weapons Tab
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getWeapScaleEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getWeaponSpeedEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getStartArmsLevelEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getEndArmsLevelEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getMovementRestrictionEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getGiveAllWeaponsEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getResidualPlayersEntry());
	GLWOptionEntry::createEntry(
		controls_, weaponsTab_, optionsGame.getDelayedDefenseActivationEntry());
	weaponsTab_->addWidget(new GLWSpacer(0.0f, 20.0f));

	GLWOptionEntry::createEntry(
		controls_, landSettingsPanel, optionsGame.getCycleMapsEntry());
	// End Add Options

	// Create buttons
	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 0.0f, 0.0f, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 0.0f, 0.0f, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, AlignRight | SpaceLeft | SpaceRight | SpaceBottom, 10.0f);

	// Layout
	setLayout(GLWPanel::LayoutVerticle);
	layout();
	landSettingsPanel->layout();

	// Add manualy placed land controls
	landList_ = new GLWIconList((landTab_->getW() - 280.0f) / 2.0f, 70.0f, 
		280.0f, landTab_->getH() - 80.0f,
		50.0f, GLWIconList::eNoDrawSelected);
	landTab_->addWidget(landList_);
	selectAllId_ = landTab_->addWidget(new GLWTextButton(
		LANG_RESOURCE("SELECT_ALL", "Select All"), 70.0f, 40.0f, 180, this,
		GLWButton::ButtonFlagCenterX))->getId();
	selectNoneId_ = landTab_->addWidget(new GLWTextButton(
		LANG_RESOURCE("SELECT_NONE", "Select None"), 270.0f, 40.0f, 180, this,
		GLWButton::ButtonFlagCenterX))->getId();
	landSettingsPanel->setX((landTab_->getW() - landSettingsPanel->getW()) / 2.0f);
}

SettingsSubSelectDialog::~SettingsSubSelectDialog()
{
}

void SettingsSubSelectDialog::selected(unsigned int id, int position)
{
	GLWIconListItem *item = landList_->getItems()[position];
	SettingsSubSelectDialogListItem *landItem = 
		(SettingsSubSelectDialogListItem *) item;
	landItem->setSelected(!landItem->getSelected());
}

void SettingsSubSelectDialog::chosen(unsigned int id, int position)
{
}

void SettingsSubSelectDialog::displayLand()
{
	OptionsGame &optionsGame = 
		SettingsSelectDialog::instance()->getOptions();

	LandscapeDefinitionsBase landscapeDefinitions;
	landscapeDefinitions.readLandscapeDefinitions();
	landList_->clear();
	landList_->setHandler(0);

	std::list<LandscapeDefinitionsEntry> &defns =
		landscapeDefinitions.getAllLandscapes();
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = defns.begin();
		 itor != defns.end();
		 ++itor)
	{
		LandscapeDefinitionsEntry &dfn = *itor;

		std::string fileName = S3D::getModFile(S3D::formatStringBuffer("data/landscapes/%s", dfn.picture.c_str()));
		if (!::S3D::fileExists(fileName))
		{
			fileName = S3D::getModFile("data/landscapes/picture-none.bmp");
		}
		bool enabled = landscapeDefinitions.landscapeEnabled(
			optionsGame, dfn.name.c_str());

		SettingsSubSelectDialogListItem *listItem =
			new SettingsSubSelectDialogListItem(
				fileName.c_str(), dfn.name.c_str(), 
				dfn.description.c_str(), enabled);
		landList_->addItem(listItem);
	}
	landList_->setHandler(this);
}

void SettingsSubSelectDialog::display()
{
	// START MOD SET
	OptionsGame &optionsGame = 
		SettingsSelectDialog::instance()->getOptions();
	S3D::setDataFileMod(optionsGame.getMod());
	displayLand();
	S3D::setDataFileMod("none");
	// END MOD SET

	GLWOptionEntry::updateControls(controls_);
	mainTab_->setDepressed();
}

void SettingsSubSelectDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		// Update all settings
		GLWOptionEntry::updateEntries(controls_);

		// Update landscape settings
		OptionsGame &optionsGame = 
			SettingsSelectDialog::instance()->getOptions();
		std::string landscapesString;
		std::vector<GLWIconListItem *> &landItems = 
			landList_->getItems();
		std::vector<GLWIconListItem *>::iterator itor;
		for (itor = landItems.begin();
			itor != landItems.end();
			++itor)
		{
			SettingsSubSelectDialogListItem *landItem = 
				(SettingsSubSelectDialogListItem *) *itor;
			if (landItem->getSelected())
			{
				if (!landscapesString.empty()) landscapesString += ":";
				landscapesString += landItem->getName();
			}
		}
		optionsGame.getLandscapesEntry().setValue(landscapesString.c_str());

		// Hide Window
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == selectAllId_ || id == selectNoneId_)
	{
		std::vector<GLWIconListItem *> &landItems = 
			landList_->getItems();
		std::vector<GLWIconListItem *>::iterator itor;
		for (itor = landItems.begin();
			itor != landItems.end();
			++itor)
		{
			SettingsSubSelectDialogListItem *landItem = 
				(SettingsSubSelectDialogListItem *) *itor;
			landItem->setSelected(id == selectAllId_);
		}
	}
}
