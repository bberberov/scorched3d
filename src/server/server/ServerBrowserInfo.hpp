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

#if !defined(__INCLUDE_ServerBrowserInfoh_INCLUDE__)
#define __INCLUDE_ServerBrowserInfoh_INCLUDE__

#include <net/NetBuffer.h>
#include <list>
#include <string>

class ServerBrowserInfo
{
public:
	static ServerBrowserInfo *instance();

	bool start();
	void processMessages();

protected:
	static ServerBrowserInfo *instance_;
	UDPsocket udpsock_;
	UDPpacket **packetV_;
	UDPpacket **packetVOut_;

	void processMessage(UDPpacket *packet);
	void processStatusMessage(std::list<std::string> &reply);
	void processPlayerMessage(std::list<std::string> &reply);
	void processInfoMessage(std::list<std::string> &reply);
	std::string addTag(const std::string &name, const std::string &value);

private:
	ServerBrowserInfo();
	virtual ~ServerBrowserInfo();
};

#endif
