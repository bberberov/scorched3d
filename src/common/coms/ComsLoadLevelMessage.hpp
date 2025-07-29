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

#ifndef _ComsLoadLevelMessage_h
#define _ComsLoadLevelMessage_h

#include <landscapedef/LandscapeDefinition.h>
#include <engine/ScorchedContext.h>
#include <simactions/SimAction.h>
#include <coms/ComsSimulateMessage.h>
#include <list>

class ComsLoadLevelMessage : public ComsMessage
{
public:
	static ComsMessageType ComsLoadLevelMessageType;

	ComsLoadLevelMessage();
	virtual ~ComsLoadLevelMessage();

	bool saveState(ScorchedContext &context);
	bool loadState(ScorchedContext &context, bool fullState = true);

	bool saveTanks(ScorchedContext &context);
	bool loadTanks(ScorchedContext &context);

	void setActualTime(fixed t) { actualTime_ = t; }
	fixed getActualTime() { return actualTime_; }

	void addSimulation(ComsSimulateMessage &simulateMessage);
	bool getSimulations(std::list<ComsSimulateMessage *> &simulateMessages);

	void setLandscapeDefinition(LandscapeDefinition &definition) { landscapeDefinition_ = definition; }
	LandscapeDefinition &getLandscapeDefinition() { return landscapeDefinition_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int emptyPosition_;
	bool lastEmpty_;
	LandscapeDefinition landscapeDefinition_;
	NetBuffer stateBuffer_;
	NetBuffer simulateBuffer_;
	NetBuffer tanksBuffer_;
	fixed actualTime_;

private:
	ComsLoadLevelMessage(const ComsLoadLevelMessage &);
	const ComsLoadLevelMessage & operator=(const ComsLoadLevelMessage &);

};

#endif //_ComsLoadLevelMessage_h

