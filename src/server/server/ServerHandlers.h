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

#if !defined(__INCLUDE_ServerHandlersh_INCLUDE__)
#define __INCLUDE_ServerHandlersh_INCLUDE__

#include <server/ServerTankChangeHandler.h>
#include <server/ServerAdminHandler.h>
#include <server/ServerBuyAccessoryHandler.h>
#include <server/ServerConnectAuthHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ServerDefenseHandler.h>
#include <server/ServerFileAkHandler.h>
#include <server/ServerGiftMoneyHandler.h>
#include <server/ServerHaveModFilesHandler.h>
#include <server/ServerInitializeModHandler.h>
#include <server/ServerLinesHandler.h>
#include <server/ServerPlayedMoveHandler.h>
#include <server/ServerOperationResultHandler.h>

class ComsMessageHandler;
class ServerHandlers
{
public:
	ServerHandlers(ComsMessageHandler &comsMessageHandler);
	virtual ~ServerHandlers();

	ServerConnectAuthHandler &getServerConnectAuthHandler() { return serverConnectAuthHandler_; }

protected:
	ServerTankChangeHandler serverTankChangeHandler_;
	ServerAdminHandler serverAdminHandler_;
	ServerBuyAccessoryHandler serverBuyAccessoryHandler_;
	ServerConnectAuthHandler serverConnectAuthHandler_;
	ServerConnectHandler serverConnectHandler_;
	ServerDefenseHandler serverDefenseHandler_;
	ServerFileAkHandler serverFileAkHandler_;
	ServerGiftMoneyHandler serverGiftMoneyHandler_;
	ServerHaveModFilesHandler serverHaveModFileHandler_;
	ServerInitializeModHandler serverInitializeModHandler_;
	ServerLinesHandler serverLinesHandler_;
	ServerPlayedMoveHandler serverPlayedMoveHandler_;
	ServerOperationResultHandler serverOperationResultHandler_;
};

#endif // __INCLUDE_ServerHandlersh_INCLUDE__
