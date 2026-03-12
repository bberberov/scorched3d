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

#ifndef __INCLUDE_Tanket_hpp_INCLUDE__
#define __INCLUDE_Tanket_hpp_INCLUDE__

#include <target/Target.hpp>

class TankAI;
class TanketType;
class TanketShotInfo;
class TanketAccessories;

class Tanket : public Target
{
public:
	// Constructor for tanket
	Tanket( ScorchedContext& context, unsigned int playerId, unsigned int destinationId, const LangString& name );
	virtual ~Tanket();

	virtual TargetType getType() { return Target::TypeTanket; }

	// Called when the state changes
	virtual void newMatch();
	virtual void newGame();

	// Serialize the tanket
	virtual bool writeMessage( NamedNetBuffer& buffer );
	virtual bool readMessage( NetBufferReader& reader );

	// clang-format off
	// uncrustify off
	TankAI*            getTankAI()                         { return tankAI_; }
	void               setTankAI( TankAI* ai );
	unsigned int       getTeam()                           { return team_; }
	void               setTeam( unsigned int team )        { team_ = team; }
	unsigned int       getDestinationId();
	void               setDestinationId( unsigned int id ) { destinationId_ = id; }
	void               setTanketType( TanketType* type )   { tanketType_ = type; }
	TanketType*        getTanketType()                     { return tanketType_; }
	TanketAccessories& getAccessories()                    { return *accessories_; }
	TanketShotInfo&    getShotInfo()                       { return *shotInfo_; }
	// uncrustify on
	// clang-format on

protected:
	ScorchedContext&   context_;
	TanketAccessories* accessories_;
	TanketShotInfo*    shotInfo_;
	TanketType*        tanketType_;
	TankAI*            tankAI_;
	unsigned int       team_;
	unsigned int       destinationId_;
};

#endif  // __INCLUDE_Tanket_hpp_INCLUDE__
