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

#include <dialogs/AdminDialog.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWDropDownText.h>
#include <GLW/GLWFont.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankColorGenerator.h>
#include <tankai/TankAINames.h>
#include <client/ScorchedClient.h>
#include <client/ClientAdminResultHandler.h>
#include <coms/ComsAdminMessage.h>
#include <coms/ComsMessageSender.h>

AdminDialog *AdminDialog::instance_ = 0;

AdminDialog *AdminDialog::instance()
{
	if (!instance_)
	{
		instance_ = new AdminDialog;
	}
	return instance_;
}

AdminDialog::AdminDialog() : 
	GLWWindow("Admin", 680.0f, 400.0f, eHideName, "")
{
	{
		playerTab_ = (GLWTab *)
			addWidget(new GLWTab("Player", LANG_RESOURCE("PLAYER", "Player"), 10, 40, 660, 330));

		// Player Table
		std::list<GLWIconTable::Column> adminColumns;
		adminColumns.push_back(GLWIconTable::Column(LANG_RESOURCE("PLAYER", "Player"), 250.0f));
		adminColumns.push_back(GLWIconTable::Column(LANG_RESOURCE("TEAM", "Team"), 100.0f));
		adminTable_ = new GLWIconTable(10.0f, 40.0f, 650.0f, 280.0f, &adminColumns, 20.0f);
		playerTab_->addWidget(adminTable_);
		adminTable_->setHandler(this);
		adminTable_->setItemCount(0);

		// Player Actions
		GLWPanel *buttonPanel = new GLWPanel(10.0f, 10.0f, 650.0f, 50.0f, false, false);
		kickButton_ = new GLWTextButton(LANG_RESOURCE("KICK", "Kick"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(kickButton_, 0, SpaceRight, 10.0f);
		banButton_ = new GLWTextButton(LANG_RESOURCE("BAN", "Ban"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(banButton_, 0, SpaceRight, 10.0f);
		slapButton_ = new GLWTextButton(LANG_RESOURCE("SLAP", "Slap"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(slapButton_, 0, SpaceRight, 10.0f);
		poorButton_ = new GLWTextButton(LANG_RESOURCE("POOR", "Poor"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(poorButton_, 0, SpaceRight, 10.0f);
		muteButton_ = new GLWTextButton(LANG_RESOURCE("MUTE", "Mute"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(muteButton_, 0, SpaceRight, 10.0f);
		unmuteButton_ = new GLWTextButton(LANG_RESOURCE("UNMUTE", "UnMute"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(unmuteButton_, 0, SpaceRight, 10.0f);
		changeNameButton_ = new GLWTextButton(LANG_RESOURCE("CHANGE_NAME", "Name"), 0, 0, 80, this, 
			GLWButton::ButtonFlagCenterX);
		buttonPanel->addWidget(changeNameButton_, 0, SpaceRight, 10.0f);
		buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
		playerTab_->addWidget(buttonPanel, 0, SpaceAll, 10.0f);
		buttonPanel->layout();
	}
	{
		botsTab_ = (GLWTab *)
			addWidget(new GLWTab("Bots", LANG_RESOURCE("BOTS", "Bots"), 10, 40, 580, 330));

		aiSelector_ = (GLWDropDown *) botsTab_->addWidget(new GLWDropDownText(10, 150, 150));
		std::list<std::string> ais;
		TankAINames::loadAIs(ais);
		std::list<std::string>::iterator itor;
		for (itor = ais.begin();
			itor != ais.end();
			++itor)
		{
			if (*itor != "Human")
			{
				aiSelector_->addEntry(GLWSelectorEntry(
					LANG_STRING(*itor), 0, false, 0, 0, *itor));
			}
		}
		addButton_ = (GLWTextButton *)
			botsTab_->addWidget(new GLWTextButton(LANG_RESOURCE("ADD", "Add"), 10, 120, 80, this, 
			GLWButton::ButtonFlagCenterX));
	}

	playerTab_->setDepressed();

	// Ok
	ok_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 500, 10, 90, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX));
}

AdminDialog::~AdminDialog()
{

}

void AdminDialog::drawColumn(unsigned int id, int row, int col,
	float x, float y, float w)
{
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();

	int pos = 0;
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor, pos++)
	{
		if (pos == row) 
		{
			if (col == 0)
			{
				GLWFont::instance()->getGameFont()->drawWidth(w, 
					GLWFont::widgetFontColor, 
					10.0f, x + 3.0f, y + 5.0f, 0.0f, 
					itor->second->getTargetName());
			}
			else
			{
				GLWFont::instance()->getGameFont()->drawWidth(w, 
					TankColorGenerator::getTeamColor(itor->second->getTeam()),
					10.0f, x + 3.0f, y + 5.0f, 0.0f, 
					TankColorGenerator::getTeamName(itor->second->getTeam()));
			}
			break;
		}
	}
}

void AdminDialog::rowSelected(unsigned int id, int row)
{

}

void AdminDialog::rowChosen(unsigned int id, int row)
{

}

void AdminDialog::columnSelected(unsigned int id, int col)
{
	
}

void AdminDialog::draw()
{
	GLWWindow::draw();

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	adminTable_->setItemCount((int) tanks.size());
}

void AdminDialog::buttonDown(unsigned int id)
{
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();

	unsigned int sid = ClientAdminResultHandler::instance()->getSid();
	unsigned int playerId = 0;
	int pos = 0;
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor, pos++)
	{
		if (adminTable_->getSelected() == pos)
		{
			playerId = itor->second->getPlayerId();
			break;
		}
	}

	if (id == ok_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == kickButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminKick, 
			S3D::formatStringBuffer("%u", playerId));
		ComsMessageSender::sendToServer(message);
	}
	else if (id == banButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminBan, 
			S3D::formatStringBuffer("%u", playerId));
		ComsMessageSender::sendToServer(message);
	}
	else if (id == slapButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminSlap, 
			S3D::formatStringBuffer("%u", playerId), "10");
		ComsMessageSender::sendToServer(message);
	}
	else if (id == poorButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminPoor,
			S3D::formatStringBuffer("%u", playerId));
		ComsMessageSender::sendToServer(message);
	}
	else if (id == muteButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminMute, 
			S3D::formatStringBuffer("%u", playerId));
		ComsMessageSender::sendToServer(message);
	}
	else if (id == unmuteButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminUnMute, 
			S3D::formatStringBuffer("%u", playerId));
		ComsMessageSender::sendToServer(message);
	}
	else if (id == addButton_->getId())
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminAdd, 
			aiSelector_->getCurrentEntry()->getDataText());
		ComsMessageSender::sendToServer(message);
	}
	else if (id == changeNameButton_->getId()) 
	{
		ComsAdminMessage message(sid, ComsAdminMessage::AdminChangeName, 
			S3D::formatStringBuffer("%u", playerId),
			"ChangeName");
		ComsMessageSender::sendToServer(message);
	}
}
