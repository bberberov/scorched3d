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

#if !defined(__INCLUDE_TargetContainerh_INCLUDE__)
#define __INCLUDE_TargetContainerh_INCLUDE__

#include <map>
#include <lang/LangString.h>

class Tank;
class Target;
class Tanket;
class TargetContainer
{
public:
	TargetContainer();
	virtual ~TargetContainer();

	void addTarget(Target *target);
	Target *removeTarget(unsigned int playerId);
	Target *getTargetById(unsigned int id);
	Tanket *getTanketById(unsigned int id);
	Tank *getTankById(unsigned int id);
	Tank *getTankByName(const LangString &name);
	
	std::map<unsigned int, Target *> &getTargets() { return targets_; }
	std::map<unsigned int, Tanket *> &getTankets() { return tankets_; }
	std::map<unsigned int, Tank *> &getTanks() { return tanks_; }

	Tank *getCurrentTank() { return currentPlayer_; }
	unsigned int getCurrentDestinationId() { return destinationId_; }
	void setCurrentDestinationId(unsigned int did) { destinationId_ = did; }
	unsigned int getCurrentPlayerId() { return playerId_; }
	void setCurrentPlayerId(unsigned int pid);
	unsigned int getCurrentRoundId() { return roundId_; }
	void setCurrentRoundId(unsigned int rid) { roundId_ = rid; }

	int aliveCount();
	int teamCount();

	int getNoOfTanks() { return tanks_.size(); }
	int getNoOfNonSpectatorTanks();

protected:
	std::map<unsigned int, Target *> targets_;
	std::map<unsigned int, Tanket *> tankets_;
	std::map<unsigned int, Tank *> tanks_;
	unsigned int playerId_;
	unsigned int destinationId_;
	unsigned int roundId_;
	Tank *currentPlayer_;
};

#endif // __INCLUDE_TargetContainerh_INCLUDE__
