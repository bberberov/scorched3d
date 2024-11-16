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

#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/SettingsSubSelectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <common/Logger.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWOptionEntry.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>

SettingsSelectDialog *SettingsSelectDialog::instance_ = 0;

SettingsSelectDialog *SettingsSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SettingsSelectDialog;
	}
	return instance_;
}

SettingsSelectDialog::SettingsSelectDialog() : 
	GLWWindow("SettingsSelect", 700.0f, 540.0f, eHideName, "")
{
	GLWPanel *controlPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);

	// Add Options
	GLWOptionEntry::createEntry(
		controls_, controlPanel, options_.getNoMaxPlayersEntry());
	GLWOptionEntry::createEntry(
		controls_, controlPanel, options_.getTeamsEntry());
	GLWOptionEntry::createEntry(
		controls_, controlPanel, options_.getTurnTypeEntry());
	GLWOptionEntry::createEntry(
		controls_, controlPanel, options_.getModEntry());
	// End Add Options

	// Gotta love layout...oooo
	controlPanel->setGridWidth(2);
	controlPanel->setLayout(GLWPanel::LayoutGrid);

	GLWPanel *topPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, true, true);
	topPanel->addWidget(controlPanel);
	GLWButton *advancedButton = new GLWTextButton(LANG_RESOURCE("ADVANCED_OPTIONS", "Advanced Options"), 
		0.0f, 0.0f, 200.0f, this,
		GLWButton::ButtonFlagCenterX);
	advancedId_ = advancedButton->getId();
	topPanel->addWidget(advancedButton, 0, AlignRight | SpaceAll, 10.0f);
	topPanel->setLayout(GLWPanel::LayoutVerticle);
	addWidget(topPanel, 0, SpaceAll, 10.0f);

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

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

SettingsSelectDialog::~SettingsSelectDialog()
{
}

void SettingsSelectDialog::display()
{
	std::string singlecustom = S3D::getSettingsFile("singlecustom.xml");
	if (S3D::fileExists(singlecustom))
	{
		if (!options_.readOptionsFromFile(singlecustom))
		{
			MsgBoxDialog::show(
				LANG_RESOURCE("FAILED_T0_LOAD_OPTIONS", "Failed to load custom options"));
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("Single custom file not found \"%s\"",
			singlecustom.c_str()));
	}

	GLWOptionEntry::updateControls(controls_);
}

void SettingsSelectDialog::buttonDown(unsigned int id)
{
	if (id == advancedId_)
	{
		GLWOptionEntry::updateEntries(controls_);

		GLWWindowManager::instance()->showWindow(
			SettingsSubSelectDialog::instance()->getId());
	}
	else if (id == okId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);

		GLWOptionEntry::updateEntries(controls_);
		options_.getNoMinPlayersEntry().setValue(options_.getNoMaxPlayers());

		std::string singlecustom = S3D::getSettingsFile("singlecustom.xml");
		options_.writeOptionsToFile(singlecustom, ClientParams::instance()->getWriteFullOptions());

		ClientParams::instance()->reset();
		ClientParams::instance()->setStartCustom(true);
		ClientMain::startClient();
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
