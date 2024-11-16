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

#if !defined(__INCLUDE_ServerRegistrationh_INCLUDE__)
#define __INCLUDE_ServerRegistrationh_INCLUDE__

#include <net/NetServerTCP.h>

class ServerRegistrationEntry : public NetMessageHandlerI
{
public:
	ServerRegistrationEntry(const char *masterListServer, 
		const char *masterListServerURI);
	virtual ~ServerRegistrationEntry();

	void start();

protected:
	const char *masterListServer_;
	bool finished_, success_;
	NetServerTCP netServer_;
	NetBuffer sendNetBuffer_;
	static int threadFunc(void *);
	void actualThreadFunc();
	bool registerGame();

	// Inherited from NetMessageHandlerI
	virtual void processMessage(NetMessage &message);
};

class ServerRegistration
{
public:
	static ServerRegistration *instance();

	void start();

protected:
	static ServerRegistration *instance_;

	ServerRegistrationEntry mainServer_;
	ServerRegistrationEntry backupServer_;

private:
	ServerRegistration();
	virtual ~ServerRegistration();
};

#endif
