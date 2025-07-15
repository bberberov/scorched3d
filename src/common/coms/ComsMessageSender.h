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

#ifndef _comsMessageSender_h
#define _comsMessageSender_h

#include <list>
#include <coms/ComsMessage.h>
#include <net/NetInterfaceFlags.h>

class ComsMessageSender
{
public:
	static bool sendToServer(
		ComsMessage &message, unsigned int flags = 0);
	static bool sendToSingleClient(
		ComsMessage &message, unsigned int destination, unsigned int flags = 0);
	static bool sendToMultipleClients(
		ComsMessage &message, std::list<unsigned int> destinations, unsigned int flags = 0);
	static bool sendToAllLoadedClients(
		ComsMessage &message, unsigned int flags = 0);
	static bool sendToAllConnectedClients(
		ComsMessage &message, unsigned int flags = 0);

protected:

	static bool formMessage(
		ComsMessage &message, unsigned int flags);

private:
	ComsMessageSender();
	virtual ~ComsMessageSender();

};

#endif // _comsMessageSender_h

