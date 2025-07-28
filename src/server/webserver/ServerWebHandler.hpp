////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_ServerWebHandlerh_INCLUDE__)
#define __INCLUDE_ServerWebHandlerh_INCLUDE__

#include <webserver/ServerWebServer.h>

namespace ServerWebHandler
{

	class PlayerHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new PlayerHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};
	class PlayerHandlerThreaded : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new PlayerHandlerThreaded(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class LogFileHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new LogFileHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class LogHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new LogHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class GameHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new GameHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class ServerHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new ServerHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class BannedHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new BannedHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class ModsHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new ModsHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class SessionsHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SessionsHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class AccountHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new AccountHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class StatsHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new StatsHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};


};

#endif // __INCLUDE_ServerWebHandlerh_INCLUDE__
