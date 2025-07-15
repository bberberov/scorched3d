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

#include <serverbrowser/ServerBrowserServerList.h>
#include <common/Defines.h>
#include <algorithm>

ServerBrowserEntry::ServerBrowserEntry()
{
}

ServerBrowserEntry::~ServerBrowserEntry()
{
}

void ServerBrowserEntry::addAttribute(const std::string &name, const std::string &value)
{
	attributes_[name] = value;
}

const char *ServerBrowserEntry::getAttribute(const std::string &attrName)
{
	static const char *empty = "";

	std::map<std::string, std::string>::iterator itor = 
		attributes_.find(attrName);
	if (itor == attributes_.end()) return empty;
	
	return (*itor).second.c_str();
}

ServerBrowserServerList::ServerBrowserServerList() :
	refreshId_(0)
{
	vectorMutex_ = SDL_CreateMutex();
}

ServerBrowserServerList::~ServerBrowserServerList()
{
	SDL_DestroyMutex(vectorMutex_);
}

struct lt_servers
{
	lt_servers(const char *name) : name_(name)
	{
	}

	bool isNumber(const char *value)
	{
		for (const char *a=value; *a; a++)
		{
			if ((*a < '0' || *a > '9') && *a != '/') return false;
		}
		return true;
	}

	bool operator()(const ServerBrowserEntry &o1, const ServerBrowserEntry &o2)
	{
		std::string o1Value = ((ServerBrowserEntry &)o1).getAttribute(name_);
		std::string o2Value = ((ServerBrowserEntry &)o2).getAttribute(name_);

		const char *v1 = o1Value.c_str();
		const char *v2 = o2Value.c_str();

		if (isNumber(v1) && isNumber(v2))
		{
			int n1 = atoi(v1);
			int n2 = atoi(v2);
			return n1 < n2;
		}

		return strcmp(v1, v2) < 0;
	}

protected:
	const char *name_;
};

void ServerBrowserServerList::sortEntries(const std::string &name)
{
	SDL_LockMutex(vectorMutex_);
	lt_servers sorter(name.c_str());
	std::sort(servers_.begin(), servers_.end(), sorter); 
	refreshId_++;
	SDL_UnlockMutex(vectorMutex_);
}

const char *ServerBrowserServerList::getEntryValue(int pos, const std::string &name)
{
	static char buffer[256];
	buffer[0] = '\0';

	SDL_LockMutex(vectorMutex_);
	if (pos >=0 && pos < (int) servers_.size())
	{
		ServerBrowserEntry &entry = servers_[pos];
		snprintf(buffer, sizeof(buffer), "%s", entry.getAttribute(name));
	}
	SDL_UnlockMutex(vectorMutex_);

	return buffer;
}

void ServerBrowserServerList::addEntryValue(int pos, 
	const std::string &name, const std::string &value)
{
	SDL_LockMutex(vectorMutex_);
	DIALOG_ASSERT(pos >=0 && pos < (int) servers_.size());
	ServerBrowserEntry &entry = servers_[pos];
	entry.addAttribute(name, value);
	refreshId_++;
	SDL_UnlockMutex(vectorMutex_);
}

int ServerBrowserServerList::getNoEntries()
{
	SDL_LockMutex(vectorMutex_);
	int size = (int) servers_.size();
	SDL_UnlockMutex(vectorMutex_);

	return size;
}

void ServerBrowserServerList::addEntry(ServerBrowserEntry &newEntry)
{
	SDL_LockMutex(vectorMutex_);
	servers_.push_back(newEntry);
	refreshId_++;
	SDL_UnlockMutex(vectorMutex_);
}

void ServerBrowserServerList::clear()
{
	SDL_LockMutex(vectorMutex_);
	servers_.clear();
	refreshId_++;
	SDL_UnlockMutex(vectorMutex_);
}

