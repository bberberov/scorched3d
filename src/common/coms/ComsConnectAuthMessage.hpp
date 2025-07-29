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

#ifndef _ComsConnectAuthMessage_h
#define _ComsConnectAuthMessage_h

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

class ComsConnectAuthMessage : public ComsMessage
{
public:
	static ComsMessageType ComsConnectAuthMessageType;

	ComsConnectAuthMessage();
	virtual ~ComsConnectAuthMessage();

	void setUserName(const char *username) { setValue("username", username); }
	void setPassword(const char *password) { setValue("password", password); }
	void setUniqueId(const char *uid) { setValue("uid", uid); }
	void setSUI(const char *SUId) { setValue("SUId", SUId); }
	void setHostDesc(const char *host) { setValue("host", host); }
	void setCompatabilityVer(unsigned int settingsver);
	void setNoPlayers(unsigned int players);

	const char *getUserName() { return getValue("username"); }
	const char *getPassword() { return getValue("password"); }
	const char *getHostDesc() { return getValue("host"); }
	const char *getUniqueId() { return getValue("uid"); }
	const char *getSUI() { return getValue("SUId"); }
	unsigned int getNoPlayers() { 
		return (unsigned int) atoi(getValue("numplayers")?getValue("numplayers"):"0"); }
	unsigned int getCompatabilityVer() { 
		return (unsigned int) atoi(getValue("compatver")?getValue("compatver"):"0"); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	std::map<std::string, std::string> values_;

	void setValue(const char *name, const char *value);
	const char *getValue(const char *name);

private:
	ComsConnectAuthMessage(const ComsConnectAuthMessage &);
	const ComsConnectAuthMessage & operator=(const ComsConnectAuthMessage &);

};

#endif // _ComsConnectAuthMessage_h

