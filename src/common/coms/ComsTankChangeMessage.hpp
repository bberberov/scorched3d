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

#ifndef __INCLUDE_ComsTankChangeMessage_hpp_INCLUDE__
#define __INCLUDE_ComsTankChangeMessage_hpp_INCLUDE__

#include <coms/ComsMessage.hpp>
#include <common/Vector.hpp>

class ComsTankChangeMessage : public ComsMessage
{
public:
	static ComsMessageType ComsTankChangeMessageType;

	ComsTankChangeMessage();
	ComsTankChangeMessage( ComsTankChangeMessage& other );
	ComsTankChangeMessage(
		unsigned int      playerId,
		const LangString& playerName,
		Vector            playerColor,
		const char*       tankType,
		const char*       modelName,
		unsigned int      destinationId,
		unsigned int      playerTeam,
		const char*       playerType,
		bool              spectate
	);
	virtual ~ComsTankChangeMessage();

	// clang-format off
	// uncrustify off
	const LangString& getPlayerName()                         { return playerName_; }
	void              setPlayerName( const LangString& name ) { playerName_ = name; }
	const char*       getPlayerType()                         { return playerType_.c_str(); }
	const char*       getModelName()                          { return modelName_.c_str(); }
	const char*       getTankType()                           { return tankType_.c_str(); }
	void              setModelName( const char* modelName )   { modelName_ = modelName; }
	unsigned int      getPlayerId()                           { return playerId_; }
	Vector&           getPlayerColor()                        { return playerColor_; }
	unsigned int      getDestinationId()                      { return destinationId_; }
	unsigned int      getPlayerTeam()                         { return playerTeam_; }
	bool              getSpectate()                           { return spectate_; }

	void        setPlayerIconName( const char* name ) { playerIconName_ = name; }
	const char* getPlayerIconName()                   { return playerIconName_.c_str(); }
	NetBuffer&  getPlayerIcon()                       { return playerIcon_; }
	// uncrustify on
	// clang-format on

	// Inherited from ComsMessage
	virtual bool writeMessage( NetBuffer& buffer );
	virtual bool readMessage( NetBufferReader& reader );

protected:
	unsigned int playerId_;
	LangString   playerName_;
	Vector       playerColor_;
	std::string  tankType_;
	std::string  modelName_;
	unsigned int destinationId_;
	unsigned int playerTeam_;
	std::string  playerType_;
	std::string  playerIconName_;
	NetBuffer    playerIcon_;
	bool         spectate_;

private:
	const ComsTankChangeMessage& operator=( const ComsTankChangeMessage& );
};

#endif  // __INCLUDE_ComsTankChangeMessage_hpp_INCLUDE__
