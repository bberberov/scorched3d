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

#if !defined(AFX_ClientSimulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_ClientSimulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#include <engine/Simulator.h>
#include <engine/GameStateI.h>
#include <coms/ComsMessageHandler.h>
#include <coms/ComsSimulateMessage.h>
#include <common/RollingAverage.h>

class ClientSimulator : 
	public GameStateI, 
	public Simulator
{
public:
	ClientSimulator();
	virtual ~ClientSimulator();

	// GameStateI
	virtual void simulate(const unsigned state, float simTime);

	// ComsMessageHandlerI
	bool processComsSimulateMessage(
		NetMessage &message,
		NetBufferReader &reader);
	bool processNetStatMessage(
		NetMessage &message,
		NetBufferReader &reader);

	virtual void newLevel();
	void setSimulationTime(fixed actualTime);
	void addComsSimulateMessage(ComsSimulateMessage &message);

	fixed getServerStepTime() { return serverStepTime_; }
	fixed getServerRoundTripTime() { return serverRoundTripTime_; }
	fixed getServerTimeDifference() { return serverTimeDifference_.getAverage(); }
	fixed getServerChoke() { return serverChoke_.getAverage(); }

	void setLoadingLevel(bool loadingLevel) { loadingLevel_ = loadingLevel; }

	class ActionControllerGameState : public GameStateI
	{
	public:
		ActionControllerGameState();

		virtual void draw(const unsigned state);
	} actionControllerGameState;

private:
	fixed waitingEventTime_;
	RollingAverage serverTimeDifference_, serverChoke_;
	fixed serverStepTime_, serverRoundTripTime_;
	bool loadingLevel_;

	virtual bool continueToSimulate();
	virtual void actualSimulate(fixed frameTime);
};

#endif // !defined(AFX_ClientSimulator_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
