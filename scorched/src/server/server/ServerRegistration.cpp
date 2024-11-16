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

#include <time.h>
#include <server/ServerRegistration.h>
#include <server/ScorchedServer.h>
#include <common/Logger.h>
#include <common/OptionsMasterListServer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

ServerRegistration *ServerRegistration::instance_ = 0;

ServerRegistration *ServerRegistration::instance()
{
	if (!instance_)
	{
		instance_ = new ServerRegistration();
	}
	return instance_;
}

ServerRegistration::ServerRegistration() :
	mainServer_(
		OptionsMasterListServer::instance()->getMasterListServer(),
		OptionsMasterListServer::instance()->getMasterListServerURI()),
	backupServer_(
		OptionsMasterListServer::instance()->getMasterListBackupServer(),
		OptionsMasterListServer::instance()->getMasterListBackupServerURI())
{
}

ServerRegistration::~ServerRegistration()
{
}

void ServerRegistration::start()
{
	mainServer_.start();
	backupServer_.start();
}

ServerRegistrationEntry::ServerRegistrationEntry(
	const char *masterListServer, 
	const char *masterListServerURI) : 
	netServer_(new NetServerHTTPProtocolSend),
	masterListServer_(masterListServer)
{
	std::string buffer = S3D::formatStringBuffer(
		"GET %s/servers.php?register=%s&port=%i HTTP/1.0\r\n"
		"User-Agent: Scorched3D\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"Cache-Control: no-cache\r\n"
		"Pragma: no-cache\r\n"
		"\r\n"
		"\r\n",
		masterListServerURI,
		ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
		ScorchedServer::instance()->getOptionsGame().getPortNo(),
		masterListServer);
	sendNetBuffer_.addDataToBuffer(buffer.c_str(), (int) buffer.size()); // Note no null

	netServer_.setMessageHandler(this);
}

ServerRegistrationEntry::~ServerRegistrationEntry()
{
}

void ServerRegistrationEntry::start()
{
	SDL_CreateThread(ServerRegistrationEntry::threadFunc, (void *) this);
}

int ServerRegistrationEntry::threadFunc(void *param)
{
	ServerRegistrationEntry *entry = 
		(ServerRegistrationEntry *) param;

	entry->actualThreadFunc();
	return 0;
}

void ServerRegistrationEntry::actualThreadFunc()
{
	const int TimeBetweenRegistrations = 540;

	for (;;)
	{
		// Ensure no connections are hanging around
		netServer_.disconnectAllClients();
		netServer_.processMessages(); // Get rid of disconnect messages

		// Register this game on the web
		success_ = false;
		finished_ = false;

		Logger::log(S3D::formatStringBuffer(
			"Connecting to registration server %s...", masterListServer_));
		if (registerGame())
		{
			time_t lastTime = time(0);
			for (;;)
			{
				// Check for any replies or timeout every 1 seconds
				SDL_Delay(1000);
				netServer_.processMessages();

				// We have recieved a disconnect
				if (finished_) break;

				// Check for timeout
				int timeOut = 
					OptionsMasterListServer::instance()->getMasterListServerTimeout();
				time_t currentTime = time(0);
				if (currentTime - lastTime > timeOut)
				{
					break;
				}
			}

			Logger::log(S3D::formatStringBuffer("Registration to %s %s.", 
				masterListServer_,
				(success_?"was successfull":"failed")));
		}
		else
		{
			Logger::log(S3D::formatStringBuffer(
				"Failed to connect to registration server %s", masterListServer_));
		}

		// Wait for TimeBetweenRegistrations seconds before registering again
		// unless we have had an error, in which case try again in 30 seconds
		int waitTime = (success_?TimeBetweenRegistrations:30);
		SDL_Delay(1000 * waitTime);
	}
}

bool ServerRegistrationEntry::registerGame()
{
	// Connect to the web server
	if (!netServer_.connect(masterListServer_, 80)) return false;

	// Send the web request
	netServer_.sendMessageServer(sendNetBuffer_);
	return true;
}

void ServerRegistrationEntry::processMessage(NetMessage &message)
{
	// We have received a reply from the web server
	if (message.getMessageType() == NetMessage::BufferMessage)
	{
		message.getBuffer().addToBuffer("");
		success_ = (strstr(message.getBuffer().getBuffer(), "success") != 0);
	}
	else if (message.getMessageType() == NetMessage::DisconnectMessage)
	{
		finished_ = true;
	}
}

