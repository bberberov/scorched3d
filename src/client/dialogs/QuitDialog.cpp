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

#include <dialogs/QuitDialog.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <engine/MainLoop.h>
#include <net/NetInterface.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <simactions/AdminSimAction.h>

QuitDialog *QuitDialog::instance_ = 0;

QuitDialog *QuitDialog::instance()
{
	if (!instance_)
	{
		instance_ = new QuitDialog;
	}
	return instance_;
}

QuitDialog::QuitDialog() : 
	GLWWindow("Quit", 210.0f, 150.0f, 0,
		"Allows the player to quit the game.")
{

}

QuitDialog::~QuitDialog()
{

}

void QuitDialog::display()
{
	GLWWindow::display();

	needCentered_ = true;
	clear();

	unsigned int state = ScorchedClient::instance()->getGameState().getState();
	bool disable = (ClientParams::instance()->getConnectedToServer() ||
		state == ClientState::StateOptions ||
		state == ClientState::StateConnect ||
		state == ClientState::StateWaitNoLandscape);
	if (!disable)
	{
		killButton_ = new GLWTextButton(LANG_RESOURCE("MASS_TANK_KILL", "Mass Tank Kill"), 0, 0, 225, this, 
			GLWButton::ButtonFlagCenterX);
		addWidget(killButton_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
		killButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("MASS_TANK_KILL", "Mass tank kill"),
			LANG_RESOURCE("MADD_TANK_KILL_TOOLTIP", "Kills all the tanks and starts the next\n"
			"round.  Only available in single player\n"
			"games.")));
	}
	else
	{
		killButton_ = 0;
	}

	disable = (state == ClientState::StateOptions || 
		state == ClientState::StateConnect);
	if (!disable)
	{
		disconnectButton_  = new GLWTextButton(LANG_RESOURCE("QUIT_GAME_TO_MENU", "Quit to Main Menu"), 0, 0, 225, this, 
			GLWButton::ButtonFlagCenterX);
		addWidget(disconnectButton_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
		disconnectButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("QUIT_GAME_TO_MENU", "Quit to Main Menu"),
			LANG_RESOURCE("QUIT_GAME_TO_MENU_TOOLTIP", "Quit current game back to game main menu")));
	}
	else
	{
		disconnectButton_ = 0;
	}

	quitButton_ = new GLWTextButton(LANG_RESOURCE("QUIT_GAME", "Quit Game"), 0, 0, 225, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	addWidget(quitButton_, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	quitButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("QUIT_GAME", "Quit Game"),
		LANG_RESOURCE("QUIT_GAME_TOOLTIP", "Quits Scorched3D back to the OS")));

	okButton_ = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 0, 0, 225, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	addWidget(okButton_, 0, SpaceLeft | SpaceRight | SpaceTop | SpaceBottom, 10.0f);
	okButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CANCEL", "Cancel"),
		LANG_RESOURCE("CANCEL_TOOLTIP", "Return to the game.")));

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

void QuitDialog::buttonDown(unsigned int id)
{
	if (id == okButton_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (killButton_ && id == killButton_->getId())
	{
		AdminSimAction *simAction = new AdminSimAction(AdminSimAction::eKillAll, 0, 0);
		if (ScorchedServer::serverStarted())
		{
			ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
		}

		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (disconnectButton_ && id == disconnectButton_->getId())
	{
		ScorchedClient::instance()->getNetInterface().disconnectAllClients();

		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == quitButton_->getId())
	{
		ScorchedClient::instance()->getMainLoop().exitLoop();
	}
}
