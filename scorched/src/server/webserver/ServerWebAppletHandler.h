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

#if !defined(__INCLUDE_ServerWebAppletHandlerh_INCLUDE__)
#define __INCLUDE_ServerWebAppletHandlerh_INCLUDE__

#include <webserver/ServerWebServer.h>

class OptionEntry;
namespace ServerWebAppletHandler
{
	class AppletFileHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new AppletFileHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class AppletHtmlHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new AppletHtmlHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class AppletActionHandler : public ServerWebServerI
	{
	public:
		virtual ServerWebServerI *createCopy() { return new AppletActionHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);
	};

	class AppletAsyncHandler : public ServerWebServerI
	{
	public:
		AppletAsyncHandler();

		virtual ServerWebServerI *createCopy() { return new AppletAsyncHandler(); }
		virtual bool processRequest(
			ServerWebServerIRequest &request,
			std::string &text);

	protected:
		unsigned int lastMessage_;
		bool initialized_;
	};
};

#endif // __INCLUDE_ServerWebAppletHandlerh_INCLUDE__
