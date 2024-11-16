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

#if !defined(__INCLUDE_ServerBrowserh_INCLUDE__)
#define __INCLUDE_ServerBrowserh_INCLUDE__

#include <serverbrowser/ServerBrowserRefresh.h>
#include <serverbrowser/ServerBrowserCollect.h>

class ServerBrowser 
{
public:
	static ServerBrowser *instance();

	enum RefreshType
	{
		RefreshNone = 0,
		RefreshLan = 1,
		RefreshNet = 2,
		RefreshFavourites = 3
	};

	bool getRefreshing() { return refreshing_; }
	void refreshList(RefreshType t);
	void cancel();
	
	ServerBrowserServerList &getServerList() { return serverList_; }
	ServerBrowserCollect &getCollect() { return serverCollector_; }

protected:
	static ServerBrowser *instance_;
	bool refreshing_;
	SDL_mutex *refreshingMutex_;

	ServerBrowserServerList serverList_;
	ServerBrowserCollect serverCollector_;
	ServerBrowserRefresh serverRefresh_;

	static int threadFunc(void *);

private:
	ServerBrowser();
	virtual ~ServerBrowser();
};

#endif
