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

#include <dialogs/HUDDialog.h>
#include <graph/OptionsDisplay.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>

HUDDialog *HUDDialog::instance_ = 0;

HUDDialog *HUDDialog::instance()
{
	if (!instance_)
	{
		instance_ = new HUDDialog;
	}
	return instance_;
}

HUDDialog::HUDDialog() :
	GLWWindow("HUD Settings", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle,
		"Allow the user to change hud settings")
{
	needCentered_ = true;

	nameBox_ = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("SHOW_NAME", "Show Name"));
	addWidget(nameBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	sightBox_ = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("SHOW_SIGHT", "Show Sight"));
	addWidget(sightBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	oldSightBox_  = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("OLD_SIGHT", "Use Old Sight Position"));
	addWidget(oldSightBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	largeSightBox_  = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("LARGE_SIGHT", "Use Visible Sight"));
	addWidget(largeSightBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	colorBox_ = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("SHOW_COLOR", "Show Color"));
	addWidget(colorBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	healthBox_ = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("SHOW_HEALTH", "Show Health"));
	addWidget(healthBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	iconBox_ = new GLWCheckBoxText(0.0f, 0.0f, LANG_RESOURCE("SHOW_ICON", "Show Icon"));
	addWidget(iconBox_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

HUDDialog::~HUDDialog()
{
}

void HUDDialog::display()
{
	GLWWindow::display();

	nameBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getDrawPlayerNames());
	sightBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getDrawPlayerSight());
	oldSightBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getOldSightPosition());
	colorBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getDrawPlayerColor());
	healthBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getDrawPlayerHealth());
	iconBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getDrawPlayerIcons());
	largeSightBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getLargeSight());
}

void HUDDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		OptionsDisplay::instance()->getDrawPlayerNamesEntry().setValue(
			nameBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getDrawPlayerSightEntry().setValue(
			sightBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getOldSightPositionEntry().setValue(
			oldSightBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getDrawPlayerColorEntry().setValue(
			colorBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getDrawPlayerHealthEntry().setValue(
			healthBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getDrawPlayerIconsEntry().setValue(
			iconBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getLargeSightEntry().setValue(
			largeSightBox_->getCheckBox().getState());
	}
	GLWWindowManager::instance()->hideWindow(getId());
}
