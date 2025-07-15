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

#include <dialogs/SkipDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <time.h>

SkipDialog *SkipDialog::instance_ = 0;

SkipDialog *SkipDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SkipDialog;
	}
	return instance_;
}

SkipDialog::SkipDialog() : 
	GLWWindow("Skip/Resign", 210.0f, 150.0f, 0,
		"Allows the player to skip or resign the current move.")
{
	resignButton_ =
		new GLWTextButton(LANG_RESOURCE("RESIGN_ROUND", "Resign Round"), 10, 45, 190, this, 
			GLWButton::ButtonFlagCenterX);

	okId_ = addWidget(new GLWTextButton(LANG_RESOURCE("SKIP MOVE", "Skip Move"), 10, 115, 190, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();
	allId_ = addWidget(new GLWTextButton(LANG_RESOURCE("SKIP ALL MOVES", "Skip All Moves"), 10, 80, 190, this, 
		GLWButton::ButtonFlagCenterX))->getId();
	resignId_ = addWidget(resignButton_)->getId();
	cancelId_ = addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();
}

SkipDialog::~SkipDialog()
{

}

void SkipDialog::display()
{
	GLWWindow::display();

	resignButton_->setEnabled(
		ScorchedClient::instance()->getOptionsGame().getResignMode() !=
		OptionsGame::ResignNone);
}

void SkipDialog::buttonDown(unsigned int id)
{
	if (id == okId_ ||
		id == allId_)
	{
		Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
		if (firstTank)
		{
			if (id == allId_)
			{
				firstTank->getState().setSkipShots(true);
			}
			TankKeyboardControlUtil::skipShot(firstTank);
		}

		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == resignId_)
	{
		Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
		if (firstTank)
		{
			TankKeyboardControlUtil::resign(firstTank);
		}

		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}

