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

#ifndef __INCLUDE_ClientSimulator_hpp_INCLUDE__
#define __INCLUDE_ClientSimulator_hpp_INCLUDE__

#include <engine/Simulator.hpp>
#include <engine/GameStateI.hpp>
#include <coms/ComsMessageHandler.hpp>
#include <coms/ComsSimulateMessage.hpp>
#include <common/RollingAverage.hpp>

class ClientSimulator : public GameStateI, public Simulator
{
public:
	ClientSimulator();
	virtual ~ClientSimulator();

	// GameStateI
	virtual void simulate( const unsigned int state, float simTime );

	// ComsMessageHandlerI
	bool processComsSimulateMessage( NetMessage& message, NetBufferReader& reader );
	bool processNetStatMessage( NetMessage& message, NetBufferReader& reader );

	virtual void newLevel();
	void         setSimulationTime( fixed actualTime );
	void         addComsSimulateMessage( ComsSimulateMessage& message );

	// clang-format off
	// uncrustify off
	fixed getServerStepTime()       { return serverStepTime_; }
	fixed getServerRoundTripTime()  { return serverRoundTripTime_; }
	fixed getServerTimeDifference() { return serverTimeDifference_.getAverage(); }
	fixed getServerChoke()          { return serverChoke_.getAverage(); }
	// uncrustify on
	// clang-format on

	void setLoadingLevel( bool loadingLevel ) { loadingLevel_ = loadingLevel; }

	class ActionControllerGameState : public GameStateI
	{
	public:
		ActionControllerGameState();

		virtual void draw( const unsigned int state );
	} actionControllerGameState;

private:
	fixed          waitingEventTime_;
	RollingAverage serverTimeDifference_, serverChoke_;
	fixed          serverStepTime_, serverRoundTripTime_;
	bool           loadingLevel_;

	virtual bool continueToSimulate();
	virtual void actualSimulate( fixed frameTime );
};

#endif  // __INCLUDE_ClientSimulator_hpp_INCLUDE__
