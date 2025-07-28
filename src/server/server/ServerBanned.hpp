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

#if !defined(__INCLUDE_ServerBannedh_INCLUDE__)
#define __INCLUDE_ServerBannedh_INCLUDE__

#include <set>
#include <map>
#include <list>
#include <lang/LangString.h>
#include <time.h>

class ServerBanned
{
public:
	ServerBanned();
	virtual ~ServerBanned();

	enum BannedType
	{
		NotBanned,
		Banned,
		Muted,
		Flagged
	};
	struct BannedEntry
	{
		time_t bantime;
		LangString name;
		std::string uniqueid;
		std::string SUI;
		std::string adminname;
		std::string reason;
		BannedType type;
	};
	struct BannedRange
	{
		unsigned int mask;
		std::map<unsigned int, BannedEntry> ips;
	};	

	std::list<BannedRange> &getBannedIps();
	BannedType getBanned(const char *unqiueid, const char *SUI);
	BannedType getBanned(unsigned int ip);
	void addBanned(unsigned int ip, 
		const LangString &name, 
		const char *uniqueId,
		const char *SUI,
		BannedType type,
		const char *adminname,
		const char *reason);
	bool save();
	bool load(bool force = false);

	static const char *getBannedTypeStr(BannedType type);

protected:
	std::list<BannedRange> bannedIps_;	
	std::map<std::string, BannedEntry> bannedIds_;
	std::map<std::string, BannedEntry> bannedSUIs_;
	time_t lastReadTime_;

	void addBannedEntry(unsigned int ip, unsigned int mask,
		const LangString &name, const char *unqiueId, const char *SUid, unsigned int bantime,
		BannedType type, const char *adminname, const char *reason);

};

#endif // __INCLUDE_ServerBannedh_INCLUDE__

