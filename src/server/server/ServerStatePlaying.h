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

#if !defined(__INCLUDE_ServerStatePlayingh_INCLUDE__)
#define __INCLUDE_ServerStatePlayingh_INCLUDE__

#include <server/ServerTurnsSequential.h>
#include <server/ServerTurnsSimultaneous.h>
#include <server/ServerTurnsFree.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <engine/SimulatorI.h>

class ServerStatePlaying
{
public:
	ServerStatePlaying();
	virtual ~ServerStatePlaying();

	bool showScore();

	void enterState();
	void simulate(fixed frameTime);

	void moveFinished(ComsPlayedMoveMessage &playedMessage);

protected:
	enum RoundState
	{
		eNone,
		eCountingDown,
		eFinished
	};

	fixed roundTime_;
	RoundState roundState_;
	unsigned int nextRoundId_;
	ServerTurns *turns_;
	ServerTurnsSequential turnsSequential_;
	ServerTurnsSimultaneous turnsSimultaneous_;
	ServerTurnsSimultaneous turnsSimultaneousNoWait_;
	ServerTurnsFree turnsFree_;
	SimulatorIAdapter<ServerStatePlaying> *roundStarted_;

	void roundStarted(fixed simulationTime, SimAction *action);
};

#endif
