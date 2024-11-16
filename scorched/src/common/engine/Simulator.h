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

#if !defined(AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#include <engine/ActionController.h>
#include <engine/EventContainer.h>
#include <engine/Wind.h>
#include <engine/SimulatorI.h>
#include <common/RandomGenerator.h>
#include <net/NetBuffer.h>

class ScorchedContext;
class Simulator
{
public:
	Simulator();
	virtual ~Simulator();

	virtual void clear();

	void setScorchedContext(ScorchedContext *context);

	virtual void simulate();

	// Accessors
	RandomGenerator &getRandomGenerator() { return random_; }
	ActionController &getActionController() { return actionController_; }
	Wind &getWind() { return wind_; }

	fixed getCurrentTime() { return currentTime_; }

	// SyncCheck
	void addSyncCheck(const std::string &msg);
	std::vector<std::string> &getSyncCheck() { return syncCheck_; }

	// Set the simulation speed
	void setFast(fixed speedMult) { speed_ = speedMult; }
	fixed getFast() { return speed_; }
protected:
	class SimActionContainer
	{
	public:
		SimActionContainer(SimAction *action, fixed fireTime, SimulatorI *callback = 0)  :
			action_(action), fireTime_(fireTime), callback_(callback) {}
		~SimActionContainer() { delete action_; }

		SimulatorI *callback_;
		SimAction *action_;
		fixed fireTime_;
	};

	unsigned int lastTickTime_;
	fixed speed_;
	fixed currentTime_, actualTime_;
	ScorchedContext *context_;
	EventContainer events_;
	Wind wind_;
	FileRandomGenerator random_;
	ActionController actionController_;
	std::list<SimActionContainer *> simActions_;
	std::vector<std::string> syncCheck_;

	virtual void simulateTime();
	virtual void actualSimulate(fixed frameTime);
	virtual bool continueToSimulate() = 0;
	virtual void newLevel();
};

#endif // !defined(AFX_Simulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
