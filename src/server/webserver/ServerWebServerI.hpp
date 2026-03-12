////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_ServerWebServerI_hpp_INCLUDE__
#define __INCLUDE_ServerWebServerI_hpp_INCLUDE__

#include <net/NetMessage.hpp>
#include <map>
#include <server/ServerAdminSessions.hpp>

class ServerWebServerIRequest
{
public:
	ServerWebServerIRequest(
		const char*                           url,
		std::map< std::string, std::string >& fields,
		std::map< std::string, NetMessage* >& parts
	);
	~ServerWebServerIRequest();

	// clang-format off
	// uncrustify off
	const char*                           getUrl()     { return url_.c_str(); }
	std::map< std::string, std::string >& getFields()  { return fields_; }
	std::map< std::string, NetMessage* >& getParts()   { return parts_; }
	ServerAdminSessions::SessionParams*   getSession() { return session_; }

	void setSession( ServerAdminSessions::SessionParams* session ) { session_ = session; }
	// uncrustify on
	// clang-format on

private:
	std::string                          url_;
	std::map< std::string, std::string > fields_;
	std::map< std::string, NetMessage* > parts_;
	ServerAdminSessions::SessionParams*  session_;
};

class ServerWebServerI
{
public:
	virtual ~ServerWebServerI();

	virtual ServerWebServerI* createCopy() = 0;

	virtual bool processRequest( ServerWebServerIRequest& request, std::string& text ) = 0;
};

#endif  // __INCLUDE_ServerWebServerI_hpp_INCLUDE__
