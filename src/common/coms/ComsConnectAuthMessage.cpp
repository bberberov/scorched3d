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

#include <coms/ComsConnectAuthMessage.h>
#include <common/Defines.h>

ComsMessageType ComsConnectAuthMessage::ComsConnectAuthMessageType("ComsConnectAuthMessageType");

ComsConnectAuthMessage::ComsConnectAuthMessage() : 
	ComsMessage(ComsConnectAuthMessageType)
{

}

ComsConnectAuthMessage::~ComsConnectAuthMessage()
{

}

void ComsConnectAuthMessage::setNoPlayers(unsigned int players)
{
	char buf[10]; snprintf(buf, 10, "%u", players); setValue("numplayers", buf);
}

void ComsConnectAuthMessage::setCompatabilityVer(unsigned int compatver)
{
	char buf[10]; snprintf(buf, 10, "%u", compatver); setValue("compatver", buf);
}

bool ComsConnectAuthMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((unsigned int) values_.size());
	std::map<std::string, std::string>::iterator itor;
	for (itor = values_.begin();
		itor != values_.end();
		++itor)
	{
		buffer.addToBuffer((*itor).first.c_str());
		buffer.addToBuffer((*itor).second.c_str());
	}

	return true;
}

bool ComsConnectAuthMessage::readMessage(NetBufferReader &reader)
{
	unsigned int noV = 0;
	values_.clear();
	if (!reader.getFromBuffer(noV)) return false;
	for (unsigned int i=0; i<noV; i++)
	{
		std::string name, value;

		if (!reader.getFromBuffer(name)) return false;
		if (!reader.getFromBuffer(value)) return false;

		// Validate the user strings
		if (strlen(value.c_str()) > 50)
		{
			((char *)value.c_str())[50] = '\0';
		}

		values_[name] = value;
	}

	return true;
}

const char *ComsConnectAuthMessage::getValue(const char *name)
{
	std::map<std::string, std::string>::iterator itor =
		values_.find(name);
	if (itor == values_.end()) return "";

	return (*itor).second.c_str();
}

void ComsConnectAuthMessage::setValue(const char *name, const char *value)
{
	values_[name] = value;
}

