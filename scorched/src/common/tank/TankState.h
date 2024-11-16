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

#if !defined(__INCLUDE_TankStateh_INCLUDE__)
#define __INCLUDE_TankStateh_INCLUDE__

#include <net/NetBuffer.h>

class Tank;
class ScorchedContext;
class TankState
{
public:
	enum State
	{
		sLoading,
		sSpectator,
		sBuying,
		sNormal,
		sDead
	};

	TankState(ScorchedContext &context, unsigned int playerId);
	virtual ~TankState();
	
	void setTank(Tank *tank) { tank_ = tank; }

	// State Modifiers
	void newGame();
	void newMatch();
	void clientNewGame();

	// State
	void setState(State s);
	State getState() { return state_; }

	bool getNotSpectator() { return notSpectator_; }
	void setNotSpectator(bool notSpectator) { notSpectator_ = notSpectator; }

	bool getTankPlaying();
	bool getTankAliveOrBuying();

	void setMuted(bool muted) { muted_ = muted; }
	bool getMuted() { return muted_; }
	void setSkipShots(bool skip) { skipshots_ = skip; }
	bool getSkipShots() { return skipshots_; }
	int getLives() { return lives_; }
	void setLives(int lives) { lives_ = lives; }
	int getMaxLives() { return maxLives_; }
	bool getNewlyJoined() { return newlyJoined_; }
	void setNewlyJoined(bool n) { newlyJoined_ = n; }
	unsigned int getStateChangeCount() { return stateChangeCount_; }
	
	const char *getStateString();
	const char *getSmallStateString();
	LangString &getSmallStateLangString();

	// Serialize the tank
	bool writeMessage(NamedNetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	Tank *tank_;
	ScorchedContext &context_;
	State state_;
	int lives_, maxLives_;
	unsigned int stateChangeCount_;
	bool muted_;
	bool skipshots_;
	bool notSpectator_;
	bool newlyJoined_;

};

#endif

