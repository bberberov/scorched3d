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

#include <igd/igd.h>
#include <common/Logger.h>
#include <XML/XMLStringBuffer.h>
#include <net/NetInterface.h>
#include <net/NetBufferUtil.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>

static const char *UPNP_MCAST_ADDR = "239.255.255.250";
static unsigned int PORT = 1900;

// Used to auto free packets
class Packet
{
public:
	Packet()
	{
		packet = SDLNet_AllocPacket(10000);
	}
	~Packet()
	{
		SDLNet_FreePacket(packet);
	}

	UDPpacket *packet;
};

igd::igd()
{
}

igd::~igd()
{
}

void igd::sendInitialRequest(int portNumber)
{
	UDPsocket udpsock = SDLNet_UDP_Open(0);
	if (!udpsock)
	{
		Logger::log("igd::Failed to create client UDP socket");
	}

	// Perform SSDP discovery
	std::list<Location> locations;
	const char *services[] = {
		"urn:schemas-upnp-org:device:InternetGatewayDevice:1",
		"urn:schemas-upnp-org:service:WANIPConnection:1",
		"urn:schemas-upnp-org:service:WANPPPConnection:1" };
	for (int i=0; i<sizeof(services)/sizeof(const char*);i++) {
		sendInitialRequest(udpsock, services[i]);
	}
	recvInitialRequest(udpsock, locations);
	SDLNet_UDP_Close(udpsock);

	// For each device discovered try to get the service definition
	std::list<Location>::iterator itor;
	for (itor = locations.begin(); itor!= locations.end(); ++itor)
	{
		// Ask the device for its services
		Location &location = *itor;
		std::string localAddress;
		sendServiceRequest(location, localAddress);
		if (parseServiceRequest(location))
		{
			// Try to use the service to add a mapping
			if (addPortMapping(location, "TCP", portNumber, localAddress) &&
				addPortMapping(location, "UDP", portNumber + 1, localAddress))
			{
				if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(), "AutoDetect"))
				{
					getExtenalAddress(location, localAddress);
				}
				break;
			}
		}
	}
}

void igd::sendInitialRequest(UDPsocket udpsock, const char *serviceType)
{
	std::string buffer = S3D::formatStringBuffer(
		"M-SEARCH * HTTP/1.1\r\n"
		"HOST: %s:%i\r\n"
		"ST: %s\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 2\r\n" // Seconds to delay response from device
		"\r\n",
		UPNP_MCAST_ADDR, PORT, 
		serviceType);

	int len = buffer.size() + 1;
	Packet packet;
	memcpy(packet.packet->data, buffer.c_str(), len);
	packet.packet->len = len;
	packet.packet->channel = -1;
	if (SDLNet_ResolveHost(&packet.packet->address, UPNP_MCAST_ADDR, PORT) != 0)
	{
		Logger::log("igd::Failed to find IGD ssdp discover address");
	}

	if (SDLNet_UDP_Send(udpsock, -1, packet.packet) == 0)
	{
		Logger::log("igd::Failed to send IGD ssdp discover");
	}
}

static bool parseInitialItem(const char *data, const char *name, std::string &result)
{
	const char *pos = strstr(data, name);
	if (!pos) return false;
	pos += strlen(name);

	const char *end = strchr(pos, '\n');
	if (!end) return false;

	result = std::string(pos, end - pos);
	return true;
}

static bool parseUrl(const char *data, std::string &host, int &port, std::string &path)
{
	const char *http = "http://";
	const char *hostPos = strstr(data, http);
	if (!hostPos) return false;
	hostPos += strlen(http);

	const char *portPos = strchr(hostPos, ':');
	if (!portPos) return false;
	((char *)portPos)[0] = '\0';
	portPos++;
	const char *urlPos = strchr(portPos, '/');
	if (!urlPos) return false;
	const char *endPos = strchr(urlPos, '\r');
	if (!endPos) return false;
	((char *)endPos)[0] = '\0';

	host = hostPos;
	path = urlPos;

	((char *)urlPos)[0] = '\0';
	port = atoi(portPos);
	return true;
}

void igd::recvInitialRequest(UDPsocket udpsock, std::list<Location> &locations)
{
	Packet packet;
	for (int i=0; i<30; i++)
	{
		SDL_Delay(100);
		if (SDLNet_UDP_Recv(udpsock, packet.packet) == 1)
		{
			const char *data = (const char *) packet.packet->data;
			if (packet.packet->len >= 0)
			{
				((char *)data)[MIN(packet.packet->len, packet.packet->maxlen - 1)] = '\0';
			}
			if (ScorchedServer::instance()->getOptionsGame().getUseUPnPLogging())
			{
				Logger::log(S3D::formatStringBuffer("igd::recieved UDP packet %s", data));
			}

			Location location;
			if (parseInitialItem(data, "\nLOCATION: ", location.location) &&
				parseInitialItem(data, "\nST: ", location.st) &&
				parseUrl(location.location.c_str(), location.host, location.port, location.path))
			{
				locations.push_back(location);
			}
		}
	}
}

void igd::sendServiceRequest(Location &location, std::string &localAddress)
{
	std::string request = S3D::formatStringBuffer(
		"GET %s HTTP/1.0\r\n"
		"Host: %s:%i\r\n"
		"Connection: Close\r\n"
		"User-Agent: UPnP/1.0, Scorched3D\r\n"
		"Cache-Control: no-cache\r\n"
		"Pragma: no-cache\r\n"
		"\r\n",
		location.path.c_str(),
		location.host.c_str(), location.port);
	sendTCPRequest(location, request, location.serviceData, localAddress);
}

bool igd::sendTCPRequest(Location &location, const std::string &request, std::string &response, std::string &localAddress)
{
	if (SDLNet_ResolveHost(&location.ipAddress, location.host.c_str(), location.port) != 0)
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to resolve host and port, %s:%i", 
			location.host.c_str(), location.port));
		return false;		
	}

	TCPsocket tcpsock = SDLNet_TCP_Open(&location.ipAddress);
	if (!tcpsock) 
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to connect to host and port, %s:%i", 
			location.host.c_str(), location.port));
		return false;		
	}
	NetBufferUtil::setBlockingIO(tcpsock);

	if (!NetBufferUtil::getLocalIPAddress(tcpsock, localAddress))
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to get local ip address"));
		SDLNet_TCP_Close(tcpsock);
		return false;	
	}

	if (ScorchedServer::instance()->getOptionsGame().getUseUPnPLogging())
	{
		Logger::log(S3D::formatStringBuffer("igd::sent TCP packet %s", request.c_str()));
	}

	if (SDLNet_TCP_Send(tcpsock, request.c_str(), request.size()) != request.size())
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to send request to host and port, %s:%i", 
			location.host.c_str(), location.port));
		SDLNet_TCP_Close(tcpsock);
		return false;	
	}

	std::string result;
	char buffer[256];
	for (;;)
	{
		int bytesRead = SDLNet_TCP_Recv(tcpsock, buffer, sizeof(buffer));
		if (bytesRead <=0) break;
		result.append(std::string(buffer, bytesRead));
	}

	if (ScorchedServer::instance()->getOptionsGame().getUseUPnPLogging())
	{
		Logger::log(S3D::formatStringBuffer("igd::recieved TCP packet %s", result.c_str()));
	}

	bool code = false;
	if (result.find("200 OK") != -1)
	{
		const char *data = strstr(result.c_str(), "\r\n\r\n");
		if (data) 
		{
			data+=4;
			response = data;
			code = true;
		} 
		else
		{
			Logger::log(S3D::formatStringBuffer("igd::Failed to get data from host and port, %s:%i, %s", 
				location.host.c_str(), location.port, result.c_str()));
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to get response from host and port, %s:%i, %s", 
			location.host.c_str(), location.port, result.c_str()));
	}

	SDLNet_TCP_Close(tcpsock);
	return code;
}

bool igd::parseServiceRequest(Location &location)
{
	if (location.serviceData.empty()) return false;

	// Parse the service document
	XMLStringBuffer xmlDocument;
	if (!xmlDocument.create(location.serviceData.c_str(), location.serviceData.size()))
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to parse service from host and port, %s:%i, %s", 
				location.host.c_str(), location.port, location.serviceData.c_str()));
		return false;
	}

	// Get the root node
	XMLNode *deviceNode = 0;
	if (!xmlDocument.getRootNode()->getNamedChild("device", deviceNode, false))
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to get service device from host and port, %s:%i, %s", 
				location.host.c_str(), location.port, location.serviceData.c_str()));
		return false;
	}

	// Get the device info
	if (!deviceNode->getNamedChild("friendlyName", location.friendlyName, false) ||
		!deviceNode->getNamedChild("manufacturer", location.manufacturer, false))
	{
		Logger::log(S3D::formatStringBuffer("igd::Failed to get service info from host and port, %s:%i, %s", 
				location.host.c_str(), location.port, location.serviceData.c_str()));
		return false;
	}
	
	Logger::log(S3D::formatStringBuffer("Found UPnP device @ %s:%i : %s (%s)", 
		location.host.c_str(), location.port,
		location.friendlyName.c_str(), location.manufacturer.c_str()));

	std::set<std::string> wantedServiceTypes;
	wantedServiceTypes.insert("urn:schemas-upnp-org:service:WANIPConnection:1");
	wantedServiceTypes.insert("urn:schemas-upnp-org:service:WANPPPConnection:1");
	if (findServiceType(deviceNode, wantedServiceTypes, location.serviceType, location.controlUrl))
	{
		Logger::log(S3D::formatStringBuffer("Found UPnP service %s", location.controlUrl.c_str()));
		return true;
	}
	return false;
}

bool igd::findServiceType(XMLNode *deviceNode, std::set<std::string> &wantedServiceTypes, 
	std::string &serviceType, std::string &controlUrl)
{
	XMLNode *serviceListNode = 0;
	if (deviceNode->getNamedChild("serviceList", serviceListNode, false)) 
	{
		XMLNode *serviceNode = 0;
		while (serviceListNode->getNamedChild("service", serviceNode, false))
		{
			if (serviceNode->getNamedChild("serviceType", serviceType, false) &&
				serviceNode->getNamedChild("controlURL", controlUrl, false))
			{
				if (wantedServiceTypes.find(serviceType) != wantedServiceTypes.end())
				{
					return true;
				}
			}
		}
	}
	XMLNode *deviceListNode = 0;
	if (deviceNode->getNamedChild("deviceList", deviceListNode, false)) 
	{
		XMLNode *newDeviceNode = 0;
		while (deviceListNode->getNamedChild("device", newDeviceNode, false))
		{
			if (findServiceType(newDeviceNode, wantedServiceTypes, serviceType, controlUrl))
			{
				return true;
			}
		}
	}
	return false;
}

bool igd::addPortMapping(Location &location, const std::string &protocol, 
	int portNumber, const std::string &localAddress)
{
	std::string data = S3D::formatStringBuffer(
		"<?xml version=\"1.0\"?>\n"
		"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:AddPortMapping xmlns:u=\"%s\">"
		"<NewRemoteHost></NewRemoteHost>"
		"<NewExternalPort>%i</NewExternalPort>"
		"<NewProtocol>%s</NewProtocol>"
		"<NewInternalPort>%i</NewInternalPort>"
		"<NewInternalClient>%s</NewInternalClient>"
		"<NewEnabled>1</NewEnabled>"
		"<NewPortMappingDescription>Scorched3DS%s</NewPortMappingDescription>"
		"<NewLeaseDuration>0</NewLeaseDuration>"
		"</u:AddPortMapping>"
		"</s:Body>"
		"</s:Envelope>",
		location.serviceType.c_str(),
		portNumber,
		protocol.c_str(),
		portNumber,
		localAddress.c_str(),
		protocol.c_str()
		);

	std::string response;
	if (sendRequest(location, "AddPortMapping", data, response))
	{
		Logger::log(S3D::formatStringBuffer("Added UPnP port mapping %s to %s:%i", 
			protocol.c_str(),
			localAddress.c_str(), portNumber));
		return true;
	}
	return false;
}

bool igd::getExtenalAddress(Location &location, const std::string &localName)
{
	std::string data = S3D::formatStringBuffer(
		"<?xml version=\"1.0\"?>\n"
		"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:GetExternalIPAddress xmlns:u=\"%s:1\">"
		"</u:GetExternalIPAddress>"
		"</s:Body>"
		"</s:Envelope>",
		location.serviceType.c_str()
		);

	std::string response;
	if (sendRequest(location, "GetExternalIPAddress", data, response))
	{
		XMLStringBuffer xmlDocument;
		if (!xmlDocument.create(response.c_str(), response.size()))
		{
			Logger::log("Failed to parse external ip address response");
			return false;
		}

		XMLNode *currentNode;
		if (!xmlDocument.getRootNode()->getNamedChild("s:Body", currentNode)) return false;
		if (!currentNode->getNamedChild("u:GetExternalIPAddressResponse", currentNode)) return false;

		std::string externalAddress;
		if (!currentNode->getNamedChild("NewExternalIPAddress", externalAddress)) return false;

		Logger::log(S3D::formatStringBuffer("Got external address %s", 
			externalAddress.c_str()));
		ScorchedServer::instance()->getOptionsGame().getMainOptions().getPublishAddressEntry().
			setValueFromString(externalAddress.c_str());
		return true;
	}
	return false;
}

bool igd::sendRequest(Location &location, const std::string &action, std::string &data, std::string &response)
{
	std::string buffer = S3D::formatStringBuffer(
		"POST %s HTTP/1.0\r\n"
		"Host: %s:%i\r\n"
		"User-Agent: UPnP/1.0, Scorched3D\r\n"
		"Content-Length: %u\r\n"
		"Content-Type: text/xml\r\n"
		"SOAPAction: \"%s#%s\"\r\n"
		"Connection: Close\r\n"
		"Cache-Control: no-cache\r\n"
		"Pragma: no-cache\r\n"
		"\r\n"
		"%s",
		location.controlUrl.c_str(),
		location.host.c_str(), location.port,
		data.size(),
		location.serviceType.c_str(),
		action.c_str(),
		data.c_str());

	std::string newLocalAddress;
	if (sendTCPRequest(location, buffer, response, newLocalAddress))
	{
		return true;
	}
	return false;
}

