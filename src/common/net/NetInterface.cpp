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

#include <net/NetInterface.h>
#include <common/Defines.h>

unsigned int NetInterface::bytesIn_ = 0;
unsigned int NetInterface::bytesOut_ = 0;
unsigned int NetInterface::pings_ = 0;
unsigned int NetInterface::connects_ = 0;

NetInterface::NetInterface()
{
}

NetInterface::~NetInterface()
{
}

const char *NetInterface::getIpName(unsigned int ipAddress)
{
	static char result[128];
	snprintf(result, 128, "%u.%u.%u.%u", 
		(ipAddress & 0xFF000000) >> 24,
		(ipAddress & 0x00FF0000) >> 16,
		(ipAddress & 0x0000FF00) >> 8,
		(ipAddress & 0x000000FF) >> 0);
	return result;
}
