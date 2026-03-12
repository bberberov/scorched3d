////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_TankAddSimAction_hpp_INCLUDE__
#define __INCLUDE_TankAddSimAction_hpp_INCLUDE__

#include <simactions/SimAction.hpp>

class TankAddSimAction : public SimAction
{
public:
	TankAddSimAction();
	TankAddSimAction(
		unsigned int       playerId,
		unsigned int       destinationId,
		const std::string& uniqueId,
		const std::string& sUID,
		const std::string& hostDesc,
		unsigned int       ipAddress,
		const LangString&  playerName,
		const std::string& aiName
	);
	virtual ~TankAddSimAction();

	virtual bool invokeAction( ScorchedContext& context );

	virtual bool writeMessage( NetBuffer& buffer );
	virtual bool readMessage( NetBufferReader& reader );

	NetBuffer& getScoreNetBuffer() { return scoreNetBuffer_; }

	static unsigned int TankAddSimActionCount;

	REGISTER_CLASS_HEADER( TankAddSimAction );

protected:
	unsigned int playerId_;
	unsigned int destinationId_;
	std::string  uniqueId_;
	std::string  sUID_;
	std::string  hostDesc_;
	unsigned int ipAddress_;
	LangString   playerName_;
	std::string  aiName_;
	NetBuffer    scoreNetBuffer_;
};

#endif  // __INCLUDE_TankAddSimAction_hpp_INCLUDE__
