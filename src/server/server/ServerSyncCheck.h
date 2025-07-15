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

#if !defined(__INCLUDE_ServerSyncCheckh_INCLUDE__)
#define __INCLUDE_ServerSyncCheckh_INCLUDE__

#include <time.h>
#include <coms/ComsSyncCheckMessage.h>
#include <coms/ComsMessageHandler.h>
#include <map>
#include <set>

class ServerSyncCheck : public ComsMessageHandlerI 
{
public:
	ServerSyncCheck(ComsMessageHandler &comsMessageHandler);
	virtual ~ServerSyncCheck();

	void enterState();
	void simulate();

	void addServerSyncCheck(ComsSyncCheckMessage *message);
	void sendAutoSyncCheck();
	void sendSyncCheck();
	void sentSyncCheck(unsigned int syncId);

	// Inherited from ComsMessageHandlerI
	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader);

protected:
	static ServerSyncCheck *instance_;
	time_t lastTime_;

	struct SyncContext
	{
		SyncContext();
		~SyncContext();

		ComsSyncCheckMessage *serverMessage;
		std::map<unsigned int, ComsSyncCheckMessage*> clientMessages;
		std::set<unsigned int> clientDestinations;
	};
	std::map<unsigned int, SyncContext*> contexts_;

	bool checkContext(SyncContext *context);
	bool compareSyncChecks(ComsSyncCheckMessage *server, 
		unsigned int destinationId, ComsSyncCheckMessage *client);
	bool compareHeightMaps(unsigned int destinationId, unsigned int syncId,
		const char *mapName,
		NetBuffer &serverBuffer, NetBuffer &clientBuffer);

};

#endif
