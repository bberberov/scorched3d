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

#if !defined(__INCLUDE_ServerWebServerUtilh_INCLUDE__)
#define __INCLUDE_ServerWebServerUtilh_INCLUDE__

#include <server/ServerAdminSessions.h>
#include <lang/LangString.h>

class NetMessage;
class OptionEntry;
namespace ServerWebServerUtil
{
	// Get query fields
	void extractMultiPartPost(const char *start, 
		const char *boundry, int sizeleft, std::map<std::string, NetMessage *> &parts);
	void extractQueryFields(std::map<std::string, std::string> &fields, char *sep);

	// General fns
	const char *strstrlen(const char *start, const char *find, int size);
	void generateSettingValue(OptionEntry *entry, std::string &value);
	const char *getField(std::map<std::string, std::string> &fields, 
		const std::string &field);
	std::string getFile(const std::string &filename);
	std::string concatLines(std::list<std::string> &lines);
	std::string htmlLangString(const LangString &langstring);

	// HTML template functions
	bool getTemplate(
		ServerAdminSessions::SessionParams *session,
		const std::string &name,
		std::map<std::string, std::string> &fields,
		std::string &result);
	bool getHtmlTemplate(
		ServerAdminSessions::SessionParams *session,
		const std::string &name,
		std::map<std::string, std::string> &fields,
		std::string &result);
	void getHtmlRedirect(
		const std::string &url,
		std::string &result);
	void getHtmlNotFound(
		std::string &result);
	bool getHtmlMessage(
		ServerAdminSessions::SessionParams *session,
		const std::string &title,
		const std::string &text,
		std::map<std::string, std::string> &fields,
		std::string &result);
};

#endif
