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

#ifndef __INCLUDE_TankAIStrings_hpp_INCLUDE__
#define __INCLUDE_TankAIStrings_hpp_INCLUDE__

#include <common/FileLines.hpp>
#include <engine/ScorchedContext.hpp>

class TankAIStrings
{
public:
	TankAIStrings();
	virtual ~TankAIStrings();

	void load();

	const char* getDeathLine( ScorchedContext& context );
	const char* getAttackLine( ScorchedContext& context );
	const char* getPlayerName();
	const char* getAIPlayerName( ScorchedContext& context );

protected:
	unsigned int playerNamesCounter_;
	unsigned int aiPlayerNamesCounter_;
	FileLines    deathLines_;
	FileLines    attackLines_;
	FileLines    playerNames_;
	FileLines    aiPlayerNames_;
};

#endif  // __INCLUDE_TankAIStrings_hpp_INCLUDE__
