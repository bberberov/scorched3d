////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_Simulator_hpp_INCLUDE__
#define __INCLUDE_Simulator_hpp_INCLUDE__

#include <engine/ActionController.hpp>
#include <engine/EventContainer.hpp>
#include <engine/Wind.hpp>
#include <engine/SimulatorI.hpp>
#include <common/RandomGenerator.hpp>
#include <net/NetBuffer.hpp>

class ScorchedContext;

class Simulator
{
public:
	Simulator();
	virtual ~Simulator();

	virtual void clear();

	void setScorchedContext( ScorchedContext* context );

	virtual void simulate();

	// Accessors
	// clang-format off
	// uncrustify off
	RandomGenerator&  getRandomGenerator()  { return random_; }
	ActionController& getActionController() { return actionController_; }
	Wind&             getWind()             { return wind_; }
	fixed             getCurrentTime()      { return currentTime_; }
	// uncrustify on
	// clang-format on

	// SyncCheck
	void addSyncCheck( const std::string& msg );

	std::vector< std::string >& getSyncCheck() { return syncCheck_; }

	// Set the simulation speed
	void setFast( fixed speedMult ) { speed_ = speedMult; }

	fixed getFast() { return speed_; }

protected:
	class SimActionContainer
	{
	public:
		SimActionContainer( SimAction* action, fixed fireTime, SimulatorI* callback = 0 )
			: action_( action )
			, fireTime_( fireTime )
			, callback_( callback )
		{}

		~SimActionContainer() { delete action_; }

		SimAction*  action_;
		fixed       fireTime_;
		SimulatorI* callback_;
	};

	unsigned int                     lastTickTime_;
	fixed                            speed_;
	fixed                            currentTime_, actualTime_;
	ScorchedContext*                 context_;
	EventContainer                   events_;
	Wind                             wind_;
	FileRandomGenerator              random_;
	ActionController                 actionController_;
	std::list< SimActionContainer* > simActions_;
	std::vector< std::string >       syncCheck_;

	virtual void simulateTime();
	virtual void actualSimulate( fixed frameTime );
	virtual bool continueToSimulate() = 0;
	virtual void newLevel();
};

#endif  // __INCLUDE_Simulator_hpp_INCLUDE__
