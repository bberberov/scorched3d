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

#include <server/ServerDestinations.h>

ServerDestinations::ServerDestinations()
{
}

ServerDestinations::~ServerDestinations()
{
	std::map<unsigned int, ServerDestination*>::iterator itor;
	for (itor = serverDestinations_.begin();
		itor != serverDestinations_.end();
		++itor)
	{
		delete itor->second;
	}
	serverDestinations_.clear();
}

void ServerDestinations::addDestination(unsigned int destinationId, unsigned int ipAddress)
{
	serverDestinations_[destinationId] = new ServerDestination(destinationId, ipAddress);
}

void ServerDestinations::removeDestination(unsigned int destinationId)
{
	ServerDestination *serverDestination = getDestination(destinationId);
	if (serverDestination)
	{
		delete serverDestination;
		serverDestinations_.erase(destinationId);
	}
}

ServerDestination *ServerDestinations::getDestination(unsigned int destinationId)
{
	std::map<unsigned int, ServerDestination*>::iterator itor =
		serverDestinations_.find(destinationId);
	if (itor == serverDestinations_.end()) return 0;
	return itor->second;
}
