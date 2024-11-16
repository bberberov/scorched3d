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

#include <dialogs/SkipAllDialog.h>
#include <engine/MainLoop.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Defines.h>
#include <time.h>

SkipAllDialog *SkipAllDialog::instance_ = 0;

SkipAllDialog *SkipAllDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SkipAllDialog;
	}
	return instance_;
}

SkipAllDialog::SkipAllDialog() : 
	GLWWindow("Skip All", 270.0f, 80.0f, eHideName, ""),
	skipAll_(false)
{
	label_ = (GLWLabel *) addWidget(new GLWLabel(10, 45));
	nowId_ = addWidget(new GLWTextButton(LANG_RESOURCE("SKIP_NOW", "Skip Now"), 10, 10, 130, this, 
		GLWButton::ButtonFlagCenterX))->getId();
	cancelId_ = addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 155, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();
}

SkipAllDialog::~SkipAllDialog()
{

}

void SkipAllDialog::simulate(float frameTime)
{
	if (skipAll_)
	{
		unsigned int currentTime = (unsigned int) time(0);
		unsigned int passedTime = currentTime - startTime_;

		if (passedTime >= 5)
		{
			Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
			if (firstTank)
			{
				TankKeyboardControlUtil::skipShot(firstTank);
			}
			GLWWindowManager::instance()->hideWindow(id_);
		}
		else
		{
			label_->setText(LANG_RESOURCE_1(
				"SKIPPING_MOVE", "Skipping move in {0}...", S3D::formatStringBuffer("%i", (5 - passedTime))));
		}
	}

	GLWWindow::simulate(frameTime);
}

void SkipAllDialog::display()
{
	startTime_ = (unsigned int) time(0);
}

void SkipAllDialog::windowInit(const unsigned state)
{
	skipAll_ = false;

	Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (firstTank)
	{
		if (firstTank->getState().getSkipShots())
		{
			skipAll_ = true;
		}
	}

	if (skipAll_)
	{
		GLWWindowManager::instance()->showWindow(id_);
	}
	else
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}

void SkipAllDialog::buttonDown(unsigned int id)
{
	if (id == cancelId_)
	{
		Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
		if (firstTank)
		{
			firstTank->getState().setSkipShots(false);
		}
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == nowId_)
	{
		startTime_ = 0;
	}
}
