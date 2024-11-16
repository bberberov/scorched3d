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

#include <coms/ComsAdminMessage.h>

ComsMessageType ComsAdminMessage::ComsAdminMessageTyper("ComsAdminMessageType");

ComsAdminMessage::ComsAdminMessage(
	unsigned int sid,
	ComsAdminMessageType type,
	const std::string &param1,
	const std::string &param2) :
	ComsMessage(ComsAdminMessageTyper),
	sid_(sid),
	type_(type),
	param1_(param1),
	param2_(param2)
{

}

ComsAdminMessage::~ComsAdminMessage()
{

}

bool ComsAdminMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(sid_);
	buffer.addToBuffer((int) type_);
	buffer.addToBuffer(param1_);
	buffer.addToBuffer(param2_);
	return true;
}

bool ComsAdminMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(sid_)) return false; 
	int t = 0;
	if (!reader.getFromBuffer(t)) return false; 
	type_ = (ComsAdminMessageType) t;
	if (!reader.getFromBuffer(param1_)) return false; 
	if (!reader.getFromBuffer(param2_)) return false; 

	return true;
}

