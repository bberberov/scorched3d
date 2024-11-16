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

#include <serverbrowser/ServerBrowser.h>
#include <common/OptionsMasterListServer.h>

ServerBrowser *ServerBrowser::instance_ = 0;

ServerBrowser *ServerBrowser::instance()
{
	if (!instance_)
	{
		instance_ = new ServerBrowser;
	}
	return instance_;
}

ServerBrowser::ServerBrowser() : 
	refreshing_(false), serverList_(), 
	serverRefresh_(serverList_),
	serverCollector_(serverList_)
{
	refreshingMutex_ = SDL_CreateMutex();
}

ServerBrowser::~ServerBrowser()	
{
	SDL_DestroyMutex(refreshingMutex_);
}

void ServerBrowser::cancel()
{
	SDL_LockMutex(refreshingMutex_);
	bool complete = true;
	if (refreshing_)
	{
		complete = false;
		serverCollector_.setCancel(true);
		serverRefresh_.setCancel(true);
	}
	SDL_UnlockMutex(refreshingMutex_);

	while (!complete)
	{
		SDL_Delay(100);
		SDL_LockMutex(refreshingMutex_);
		if (!refreshing_) complete = true;
		SDL_UnlockMutex(refreshingMutex_);
	}
}

void ServerBrowser::refreshList(RefreshType t)
{
	bool alreadyRefreshing = false;
	SDL_LockMutex(refreshingMutex_);
	alreadyRefreshing = refreshing_;
	refreshing_ = true;
	SDL_UnlockMutex(refreshingMutex_);
	if (alreadyRefreshing) return;

	SDL_CreateThread(ServerBrowser::threadFunc, (void *) int(t));
}

int ServerBrowser::threadFunc(void *var)
{
	RefreshType typ = (RefreshType) long(var);

	bool result = false;
	switch (typ)
	{
	case RefreshNone:
		// No new list
		break;
	case RefreshLan:
		result = instance_->serverCollector_.fetchLANList();
		break;
	case RefreshNet:
		result = 
			instance_->serverCollector_.fetchServerList(
				OptionsMasterListServer::instance()->getMasterListServer(),
				OptionsMasterListServer::instance()->getMasterListServerURI()) 
			||
			instance_->serverCollector_.fetchServerList(
				OptionsMasterListServer::instance()->getMasterListBackupServer(),
				OptionsMasterListServer::instance()->getMasterListBackupServerURI());

		break;
	case RefreshFavourites:
		result = instance_->serverCollector_.fetchFavoritesList();
		break;
	}
	if (result)
	{
		instance_->serverRefresh_.refreshList();
	}
	
	SDL_LockMutex(instance_->refreshingMutex_);
	instance_->serverCollector_.setCancel(false);
	instance_->serverRefresh_.setCancel(false);
	instance_->refreshing_ = false;
	SDL_UnlockMutex(instance_->refreshingMutex_);
	return 0;
}
