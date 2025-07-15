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

#include <coms/ComsMessage.h>
#include <map>
#include <vector>

static std::map<std::string, ComsMessageType *> *coms_message_map = 0;
static std::vector<ComsMessageType *> *coms_message_array = 0;

ComsMessageType::ComsMessageType(const std::string &name) :
	name_(name), id_(0)
{
	DIALOG_ASSERT(!coms_message_array);
	if (!coms_message_map) {
		coms_message_map = new std::map<std::string, ComsMessageType *>();
	}
	(*coms_message_map)[name] = this;
}

ComsMessageType::~ComsMessageType()
{
}

unsigned int ComsMessageType::getId()
{
	if (!coms_message_array) getTypeForId(1);
	return id_; 
}

ComsMessageType *ComsMessageType::getTypeForId(unsigned int id)
{
	if (!coms_message_array)
	{
		coms_message_array = new std::vector<ComsMessageType *>();
		coms_message_array->resize(coms_message_map->size());

		int id = 0;
		std::map<std::string, ComsMessageType *>::iterator itor;
		for (itor = coms_message_map->begin();
			itor != coms_message_map->end();
			++itor, id++)
		{
			(*coms_message_array)[id] = itor->second;
			itor->second->id_ = id;
		}
		delete coms_message_map;
		coms_message_map = 0;
	}
	if (id >= coms_message_array->size()) return 0;
	return (*coms_message_array)[id];
}

ComsMessage::ComsMessage(ComsMessageType &messageType) : 
	messageType_(messageType)
{

}

ComsMessage::~ComsMessage()
{

}

bool ComsMessage::writeTypeMessage(NetBuffer &buffer)
{
	unsigned char actualId = messageType_.getId();
	DIALOG_ASSERT(actualId == messageType_.getId());
	buffer.addToBuffer(actualId);
	return true;
}

bool ComsMessage::readTypeMessage(NetBufferReader &reader)
{
	unsigned char type;
	if (!reader.getFromBuffer(type)) return false;
	return true;
}
