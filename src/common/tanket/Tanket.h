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

#if !defined(AFX_TANKET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
#define AFX_TANKET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_

#include <target/Target.h>

class TankAI;
class TanketType;
class TanketShotInfo;
class TanketAccessories;
class Tanket : public Target
{
public:
	// Constructor for tanket
	Tanket(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const LangString &name);
	virtual ~Tanket();

	virtual TargetType getType() { return Target::TypeTanket; }

	// Called when the state changes
	virtual void newMatch();
	virtual void newGame();

	// Serialize the tanket
	virtual bool writeMessage(NamedNetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

	TankAI *getTankAI() { return tankAI_; }
	void setTankAI(TankAI *ai);
	unsigned int getTeam() { return team_; }
	void setTeam(unsigned int team) { team_ = team; }
	unsigned int getDestinationId();
	void setDestinationId(unsigned int id) { destinationId_ = id; }

	void setTanketType(TanketType *type) { tanketType_ = type; }
	TanketType *getTanketType() { return tanketType_; }
	TanketAccessories& getAccessories() { return *accessories_; }
	TanketShotInfo& getShotInfo() { return *shotInfo_; }

protected:
	ScorchedContext &context_;
	TanketAccessories *accessories_;
	TanketShotInfo *shotInfo_;
	TanketType *tanketType_;
	TankAI *tankAI_;
	unsigned int team_;
	unsigned int destinationId_;

};

#endif // !defined(AFX_TANKET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
