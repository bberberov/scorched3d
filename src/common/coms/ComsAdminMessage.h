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

#if !defined(__INCLUDE_ComsAdminMessageh_INCLUDE__)
#define __INCLUDE_ComsAdminMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsAdminMessage : public ComsMessage
{
public:
	static ComsMessageType ComsAdminMessageTyper;

	enum ComsAdminMessageType
	{
		AdminNone,
		AdminLogin,
		AdminLoginLocal,
		AdminLogout,
		AdminShow,
		AdminShowBanned,
		AdminChangeName,
		AdminSyncCheck,
		AdminKillAll,
		AdminKick,
		AdminSlap,
		AdminPoor,
		AdminBan,
		AdminFlag,
		AdminMute,
		AdminPermMute,
		AdminUnPermMute,
		AdminUnMute,
		AdminAdminTalk,
		AdminTalk,
		AdminMessage,
		AdminNewGame,
		AdminAdd,
		AdminKill
	};

	ComsAdminMessage(
		unsigned int sid = 0,
		ComsAdminMessageType type = AdminNone, 
		const std::string &param1_ = "",
		const std::string &param2_ = "");
	virtual ~ComsAdminMessage();

	unsigned int getSid() { return sid_; }
	ComsAdminMessageType getType() { return type_; }
	const char *getParam1() { return param1_.c_str(); }
	const char *getParam2() { return param2_.c_str(); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int sid_;
	ComsAdminMessageType type_;
	std::string param1_;
	std::string param2_;

private:
	ComsAdminMessage(const ComsAdminMessage &);
	const ComsAdminMessage & operator=(const ComsAdminMessage &);

};

#endif // __INCLUDE_ComsAdminMessageh_INCLUDE__

