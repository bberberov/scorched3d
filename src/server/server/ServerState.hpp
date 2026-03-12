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

#ifndef __INCLUDE_ServerState_hpp_INCLUDE__
#define __INCLUDE_ServerState_hpp_INCLUDE__

#include <server/ServerStateEnoughPlayers.hpp>
#include <server/ServerStateNewGame.hpp>
#include <server/ServerStateTankNewGame.hpp>
#include <server/ServerStateStartingMatch.hpp>
#include <server/ServerStateBuying.hpp>
#include <server/ServerStatePlaying.hpp>
#include <server/ServerStateScore.hpp>
#include <server/ServerStateFinishWait.hpp>

class ServerState
{
public:
	ServerState();
	virtual ~ServerState();

	enum ServerStateEnum
	{
		ServerStartupState = 1,
		ServerWaitingForPlayersState,
		ServerMatchCountDownState,
		ServerNewLevelState,
		ServerBuyingState,
		ServerTankNewGameState,
		ServerPlayingState,
		ServerFinishWaitState,
		ServerScoreState
	};

	void simulate( fixed frameTime );

	// clang-format off
	// uncrustify off
	ServerStateEnum getState()                        { return serverState_; }
	void            setState( ServerStateEnum state ) { serverState_ = state; }
	// uncrustify on
	// clang-format on

	void buyingFinished( ComsPlayedMoveMessage& message );
	void moveFinished( ComsPlayedMoveMessage& message );
	void scoreFinished();

protected:
	ServerStateEnum serverState_;

	ServerStateEnoughPlayers enoughPlayers_;
	ServerStateNewGame       newGame_;
	ServerStateStartingMatch startingMatch_;
	ServerStateBuying        buying_;
	ServerStatePlaying       playing_;
	ServerStateScore         score_;
	ServerStateFinishWait    finishWait_;
	ServerStateTankNewGame   tankNewGame_;
};

#endif  // __INCLUDE_ServerState_hpp_INCLUDE__
