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

#include <dialogs/ConnectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <dialogs/ProgressDialog.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientState.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsConnectMessage.h>
#include <net/NetInterface.h>
#include <common/Logger.h>

ConnectDialog *ConnectDialog::instance_ = 0;

ConnectDialog *ConnectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ConnectDialog;
	}
	return instance_;
}

ConnectDialog::ConnectDialog() : 
	GLWWindow("Connect", -100.0f, 10.0f, 20.0f, 20.0f, eNoDraw | eNoTitle,
		"Connection dialog"),
	connectionState_(eWaiting),
	tryCount_(0), lastTime_(0), idStore_(0),
	remoteConnectionThread_(0)
{
}

ConnectDialog::~ConnectDialog()
{
}

UniqueIdStore &ConnectDialog::getIdStore()
{
	if (!idStore_)
	{
		idStore_ = new UniqueIdStore();
		// Get the unique id
		if (!idStore_->loadStore())
		{
			Logger::log("Failed to load id store");
		}
	}
	return *idStore_;
}

void ConnectDialog::windowInit(const unsigned state)
{
}

void ConnectDialog::start()
{
	connectionState_ = eWaiting;
	tryCount_ = 0;
	lastTime_ = 0;
}

void ConnectDialog::simulate(float frameTime)
{
	time_t currentTime = time(0);
	if (connectionState_ == eWaiting)
	{
		if (currentTime - lastTime_ > 3)
		{
			ScorchedClient::instance()->getNetInterface().stop();

			if (tryCount_<3)
			{
				tryCount_++;
				lastTime_ = currentTime;
				tryConnection();
			}
			else
			{
				finished();

				LangString msg = LANG_RESOURCE_2("FAILED_TO_CONNECT_TIMEOUT",
					"Failed to connect to server \"{0}:{1}\", timeout.",
					host_,
					port_);
				MsgBoxDialog::show(msg);

				ScorchedClient::instance()->getGameState().stimulate(
					ClientState::StimOptions);
			}
		}
	}
}

void ConnectDialog::tryConnection()
{
	const char *serverName = 
		(ClientParams::instance()->getConnect()[0]?
		ClientParams::instance()->getConnect():
		"Localhost");

	host_ = serverName;
	port_ = S3D::ScorchedPort;

	char *colon = strchr((char *)serverName, ':');
	if (colon) 
	{
		char *stop;
		*colon = '\0';
		colon++;
		port_ = strtol(colon, &stop, 10);
		host_ = serverName;
		colon--;
		*colon = ':';
	}	

	ProgressDialog::instance()->progressChange(
		LANG_RESOURCE_3("CONNECTING_TO", 
			"Connecting to \"{0}:{1}\" ({3})....", 
			host_, 
			S3D::formatStringBuffer("%i", port_), 
			S3D::formatStringBuffer("%i", tryCount_)), 0);

	connectionState_ = eTryingConnection;
	if (ClientParams::instance()->getConnectedToServer())
	{
		// Do in a thread so connect can block if it wants!
		remoteConnectionThread_ = SDL_CreateThread(ConnectDialog::tryRemoteConnection, 0);
	}
	else
	{
		// Or connect localy
		tryLocalConnection();
	}
}

int ConnectDialog::tryRemoteConnection(void *)
{
	char *host = (char *) instance_->host_.c_str();
	int port = instance_->port_;

	// Try to connect to the server
	ScorchedClient::instance()->getNetInterface().connect(host, port);

	// Wait for result
	instance_->connectionState_ = eWaiting;
	return 0;
}

void ConnectDialog::tryLocalConnection()
{
	// Try to connect localy
	ScorchedClient::instance()->getNetInterface().connect("Local", 0);

	// Wait for result
	connectionState_ = eWaiting;;
}

void ConnectDialog::connected()
{
	ProgressDialog::instance()->progressChange(LANG_RESOURCE("CONNECTED", "Connected"), 100);

	// Wait for the coms to start
	for (int i=0; i<10 && !ScorchedClient::instance()->getNetInterface().started(); i++)
	{
		SDL_Delay(500);
	}

	// If we connected then send our details to the server
	ComsConnectMessage connectMessage;
	connectMessage.setVersion(S3D::ScorchedVersion.c_str());
	connectMessage.setProtocolVersion(S3D::ScorchedProtocolVersion.c_str());

	if (!ComsMessageSender::sendToServer(connectMessage))
	{
		ScorchedClient::instance()->getNetInterface().stop();

		LangString msg = LANG_RESOURCE_2("FAILED_TO_CONNECT_SEND",
			"Failed to connect to server \"{0}:{1}\", send failed.",
			host_,
			port_);
		MsgBoxDialog::show(msg);

		ScorchedClient::instance()->getNetInterface().stop();
		ScorchedClient::instance()->getGameState().stimulate(
			ClientState::StimOptions);
	}
	finished();
}

void ConnectDialog::finished()
{
	if (remoteConnectionThread_)
	{
		int status = 0;
		SDL_WaitThread(remoteConnectionThread_, &status);
		remoteConnectionThread_ = 0;
	}
	connectionState_ = eFinished;
}
