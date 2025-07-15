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

#include <coms/ComsSimulateMessage.h>

ComsMessageType ComsSimulateMessage::ComsSimulateMessageType("ComsSimulateMessageType");

ComsSimulateMessage::ComsSimulateMessage() :
	ComsMessage(ComsSimulateMessageType)
{
}

ComsSimulateMessage::ComsSimulateMessage(fixed eventTime, fixed actualTime,
	unsigned int serverTime,
	std::list<SimAction *> &actions) :
	ComsMessage(ComsSimulateMessageType),
	eventTime_(eventTime), actualTime_(actualTime), serverTime_(serverTime)
{
	actions_.insert(actions_.begin(), actions.begin(), actions.end());
}

ComsSimulateMessage::~ComsSimulateMessage()
{
}

bool ComsSimulateMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(eventTime_);
	buffer.addToBuffer(actualTime_);
	buffer.addToBuffer(serverTime_);
	buffer.addToBuffer((unsigned int) actions_.size());
	std::list<SimAction *>::iterator itor;
	for (itor = actions_.begin();
		itor != actions_.end();
		++itor)
	{
		SimAction *action = *itor;
		buffer.addToBuffer(action->getClassName());
		action->writeMessage(buffer);
	}
	return true;
}

bool ComsSimulateMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(eventTime_)) return false;
	if (!reader.getFromBuffer(actualTime_)) return false;
	if (!reader.getFromBuffer(serverTime_)) return false;
	unsigned int actionCount = 0;
	if (!reader.getFromBuffer(actionCount)) return false;
	for (unsigned int a=0; a<actionCount; a++)
	{
		std::string actionName;
		if (!reader.getFromBuffer(actionName)) return false;
		SimAction *action = (SimAction *) MetaClassRegistration::
			getNewClass(actionName.c_str());
		if (!action) return false;
		if (!action->readMessage(reader)) return false;
		actions_.push_back(action);		
	}
	return true;
}
