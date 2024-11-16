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

#if !defined(__INCLUDE_ServerWebSettingsHandlerh_INCLUDE__)
#define __INCLUDE_ServerWebSettingsHandlerh_INCLUDE__

#include <webserver/ServerWebServer.h>

class OptionEntry;
namespace ServerWebSettingsHandler
{
	class SettingsAllHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SettingsAllHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class SettingsPlayersHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SettingsPlayersHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class SettingsLandscapeHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SettingsLandscapeHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class SettingsMainHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SettingsMainHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class SettingsModHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new SettingsModHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};
};

#endif // __INCLUDE_ServerWebSettingsHandlerh_INCLUDE__
