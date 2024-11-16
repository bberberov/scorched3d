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

#include <dialogs/AdminCheckDialog.h>
#include <dialogs/AdminDialog.h>
#include <dialogs/AdminAuthDialog.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientChannelManager.h>
#include <coms/ComsAdminMessage.h>
#include <coms/ComsMessageSender.h>

AdminCheckDialog *AdminCheckDialog::instance_ = 0;

AdminCheckDialog *AdminCheckDialog::instance()
{
	if (!instance_)
	{
		instance_ = new AdminCheckDialog;
	}
	return instance_;
}

AdminCheckDialog::AdminCheckDialog() :
	GLWWindow("Admin", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle, "Add/Remove players.")
{
	ClientAdminResultHandler::instance()->addHandler(this);
	needCentered_ = true;
}

AdminCheckDialog::~AdminCheckDialog()
{
}

void AdminCheckDialog::display()
{
	GLWWindow::display();

	unsigned int sid = ClientAdminResultHandler::instance()->getSid();
	ComsAdminMessage message(sid, ComsAdminMessage::AdminLoginLocal, "", "");
	ComsMessageSender::sendToServer(message);

	GLWWindowManager::instance()->hideWindow(getId());
}

void AdminCheckDialog::adminResult(unsigned int sid, ComsAdminMessage::ComsAdminMessageType type)
{
	GLWWindowManager::instance()->hideWindow(AdminDialog::instance()->getId());
	GLWWindowManager::instance()->hideWindow(AdminAuthDialog::instance()->getId());
	if (sid)
	{
		ClientChannelManager::instance()->addChannel("general", "admin");
		GLWWindowManager::instance()->showWindow(AdminDialog::instance()->getId());
	}
	else 
	{
		ClientChannelManager::instance()->removeChannel("admin");
		if (type == ComsAdminMessage::AdminLogin ||
			type == ComsAdminMessage::AdminLoginLocal)
		{
			GLWWindowManager::instance()->showWindow(AdminAuthDialog::instance()->getId());
		}
	}
}
