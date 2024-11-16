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

#if !defined(__INCLUDE_OptionsTransienth_INCLUDE__)
#define __INCLUDE_OptionsTransienth_INCLUDE__

#include <common/Vector.h>
#include <common/OptionsScorched.h>
#include <net/NetBuffer.h>

class TargetContainer;
class OptionsTransient
{
public:
	OptionsTransient(OptionsScorched &optionsGame);
	virtual ~OptionsTransient();

	unsigned int getLeastUsedTeam(TargetContainer &container);

	enum WallType
	{
		wallConcrete = 0,
		wallBouncy = 1,
		wallWrapAround = 2,
		wallNone = 3
	};
	enum WallSide
	{
		LeftSide = 0,
		RightSide,
		TopSide,
		BotSide
	};

	void reset();
	void newGame();

	void startNewGame();

	// Walls
	WallType getWallType() { return (WallType) wallType_.getValue(); }
	Vector &getWallColor();

	// Rounds left
	int getCurrentRoundNo() { return currentRoundNo_.getValue(); }
	void setCurrentRoundNo(int round) { currentRoundNo_.setValue(round); }

	int getCurrentTurnNo() { return currentTurnNo_.getValue(); }
	void setCurrentTurnNo(int round) { currentTurnNo_.setValue(round); }

	// Arms Level
	int getArmsLevel();

	// Used to send this structure over coms
	bool writeToBuffer(NetBuffer &buffer);
	bool readFromBuffer(NetBufferReader &reader);

	std::list<OptionEntry *> &getOptions() { return options_; }

protected:
	std::list<OptionEntry *> options_;
	OptionsScorched &optionsGame_;
	OptionEntryInt currentRoundNo_, currentTurnNo_;
	OptionEntryInt wallType_;

	bool newGame_;
	void newGameWall();

};

#endif
