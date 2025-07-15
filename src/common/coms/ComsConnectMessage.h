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

#ifndef _comsConnectMessage_h
#define _comsConnectMessage_h

// The very first message sent from the client to the server
// requesting a connection.
//
// Since version 37 the format of this message has been changed
// it is now sent/serialized as a name/value pair list
// this means we can add any extra args can be added without breaking
// backward compatability

#include <coms/ComsMessage.h>
#include <stdlib.h>
#include <map>
#include <string>

class ComsConnectMessage : public ComsMessage
{
public:
	static ComsMessageType ComsConnectMessageType;

	ComsConnectMessage();
	virtual ~ComsConnectMessage();

	void setVersion(const char *version) { setValue("version", version); }
	void setProtocolVersion(const char *pversion) { setValue("pversion", pversion); }

	const char *getVersion() { return getValue("version"); }
	const char *getProtocolVersion() { return getValue("pversion"); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	std::map<std::string, std::string> values_;

	void setValue(const char *name, const char *value);
	const char *getValue(const char *name);

private:
	ComsConnectMessage(const ComsConnectMessage &);
	const ComsConnectMessage & operator=(const ComsConnectMessage &);

};

#endif // _comsConnectMessage_h

