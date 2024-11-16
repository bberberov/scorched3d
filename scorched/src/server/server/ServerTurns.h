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

#if !defined(__INCLUDE_ServerTurnsh_INCLUDE__)
#define __INCLUDE_ServerTurnsh_INCLUDE__

#include <engine/SimulatorI.h>
#include <list>
#include <map>

class Tanket;
class ComsPlayedMoveMessage;
class fixed;
class ServerTurns 
{
public:
	ServerTurns(bool waitForShots);
	virtual ~ServerTurns();

	// Called by ServerState
	virtual void enterState();
	virtual void simulate(fixed frameTime);
	virtual bool finished();
	virtual void moveFinished(ComsPlayedMoveMessage &playedMessage);

	// Called by This Class
	virtual void internalEnterState() = 0;
	virtual void internalSimulate(fixed frameTime) = 0;
	virtual bool internalFinished();
	virtual void internalMoveFinished(ComsPlayedMoveMessage &playedMessage) = 0;
	virtual void internalShotsFinished();

protected:
	enum ShotsState
	{
		eShotsNone,
		eShotsWaitingStart,
		eShotsWaitingEnd
	};
	struct PlayingPlayer
	{
		PlayingPlayer(
			unsigned int moveId,
			fixed moveTime) :
			startedMove_(false),
			moveId_(moveId),
			moveTime_(moveTime)
		{
		}

		unsigned int moveId_;
		bool startedMove_;
		fixed moveTime_;
	};

	unsigned int nextNonNormalMoveId_;
	bool waitForShots_;
	ShotsState shotsState_;
	SimulatorIAdapter<ServerTurns> *shotsStarted_, *moveStarted_;
	std::map<unsigned int, PlayingPlayer*> playingPlayers_;
	std::map<unsigned int, fixed> timedPlayers_;

	bool showScore();
	void playMove(Tanket *tank, unsigned int moveId, fixed maximumShotTime);
	void playMoveFinished(Tanket *tank);
	void playShots(std::list<ComsPlayedMoveMessage *> messages, unsigned int moveId, 
		bool timeOutPlayers, bool referenced);
	void shotsStarted(fixed simulationTime, SimAction *action);
	void moveStarted(fixed simulationTime, SimAction *action);
	void incrementTurn();
};

#endif
