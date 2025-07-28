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

#if !defined(AFX_igd_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_igd_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <net/NetBuffer.h>
#include <XML/XMLNode.h>
#include <list>
#include <set>

class igd
{
public:
	igd();
	virtual ~igd();

	void sendInitialRequest(int portNumber);

protected:
	struct Location
	{
		std::string friendlyName, manufacturer;
		std::string location, st;
		std::string host, path;
		int port;
		IPaddress ipAddress;
		std::string serviceData;
		std::string controlUrl;
		std::string serviceType;
	};

	void sendInitialRequest(UDPsocket udpsock, const char *serviceType);
	void recvInitialRequest(UDPsocket udpsock, std::list<Location> &locations);
	void sendServiceRequest(Location &location, std::string &localAddress);
	bool parseServiceRequest(Location &location);
	bool sendTCPRequest(Location &location, const std::string &request, std::string &response, std::string &localAddress);
	bool findServiceType(XMLNode *deviceNode, std::set<std::string> &wantedServiceTypes,
		std::string &serviceType, std::string &controlUrl);
	bool addPortMapping(Location &location, const std::string &protocol,
		int portNumber, const std::string &localName);
	bool getExtenalAddress(Location &location, const std::string &localName);
	bool sendRequest(Location &location, const std::string &action, std::string &data, std::string &response);
};

#endif // !defined(AFX_igd_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
