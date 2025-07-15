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

#ifndef _ComsSimulateMessage_h
#define _ComsSimulateMessage_h

#include <coms/ComsMessage.h>
#include <simactions/SimAction.h>
#include <list>

class ComsSimulateMessage : public ComsMessage
{
public:
	static ComsMessageType ComsSimulateMessageType;

	ComsSimulateMessage();
	ComsSimulateMessage(fixed eventTime, fixed actualTime, 
		unsigned int serverTime,
		std::list<SimAction *> &actions);
	virtual ~ComsSimulateMessage();

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

	fixed &getEventTime() { return eventTime_; }
	fixed &getActualTime() { return actualTime_; }
	unsigned int getServerTime() { return serverTime_; }
	std::list<SimAction *> &getActions() { return actions_; }

protected:
	fixed eventTime_, actualTime_;
	unsigned int serverTime_;
	std::list<SimAction *> actions_;

private:
	ComsSimulateMessage(const ComsSimulateMessage &);
	const ComsSimulateMessage & operator=(const ComsSimulateMessage &);

};

#endif //_ComsSimulateMessage_h

