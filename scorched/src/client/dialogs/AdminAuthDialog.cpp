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

#include <dialogs/AdminAuthDialog.h>
#include <dialogs/AdminDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientAdminResultHandler.h>
#include <coms/ComsAdminMessage.h>
#include <coms/ComsMessageSender.h>

AdminAuthDialog *AdminAuthDialog::instance_ = 0;

AdminAuthDialog *AdminAuthDialog::instance()
{
	if (!instance_)
	{
		instance_ = new AdminAuthDialog;
	}
	return instance_;
}

AdminAuthDialog::AdminAuthDialog() :
	GLWWindow("Admin Auth", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle | eHideName, "")
{
	needCentered_ = true;
}

AdminAuthDialog::~AdminAuthDialog()
{
}

void AdminAuthDialog::display()
{
	GLWWindow::display();

	clear();

	addWidget(
		new GLWLabel(0.0f, 0.0f, LANG_RESOURCE("ADMINAUTH_LINE", "Admin authentication.")), 
		0, SpaceAll, 10.0f);

	GLWPanel *inputPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);

	username_ = 0;
	{
		username_ = new GLWTextBox(0.0f, 0.0f, 200.0f);
		inputPanel->addWidget(new GLWLabel(0.0f, 0.0f, LANG_RESOURCE("USER_NAME_LABEL", "User Name :")));
		inputPanel->addWidget(username_, 0, SpaceLeft | SpaceTop, 10.0f);
	}
	password_ = 0;
	{
		password_ = new GLWTextBox(0.0f, 0.0f, 200.0f, LangString(), GLWTextBox::eFlagPassword);
		inputPanel->addWidget(new GLWLabel(0.0f, 0.0f, LANG_RESOURCE("PASSWORD_LABEL", "Password :")));
		inputPanel->addWidget(password_, 0, SpaceLeft | SpaceTop, 10.0f);
	}
	inputPanel->setGridWidth(2);
	inputPanel->setLayout(GLWPanel::LayoutGrid);
	addWidget(inputPanel, 0, SpaceAll, 10.0f);

	username_->setCurrent();

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

void AdminAuthDialog::buttonDown(unsigned int id)
{
	GLWWindowManager::instance()->hideWindow(getId());
	if (id == okId_)
	{
		unsigned int sid = ClientAdminResultHandler::instance()->getSid();
		ComsAdminMessage message(sid, ComsAdminMessage::AdminLogin, 
			username_->getText().c_str(), 
			password_->getText().c_str());
		ComsMessageSender::sendToServer(message);
	}
}
