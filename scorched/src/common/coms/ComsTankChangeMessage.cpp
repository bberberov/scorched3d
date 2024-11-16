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

#include <coms/ComsTankChangeMessage.h>

ComsMessageType ComsTankChangeMessage::ComsTankChangeMessageType("ComsTankChangeMessageType");

ComsTankChangeMessage::ComsTankChangeMessage() :
	ComsMessage(ComsTankChangeMessageType)
{
}

ComsTankChangeMessage::ComsTankChangeMessage(ComsTankChangeMessage &other) :
	ComsMessage(ComsTankChangeMessageType),
	playerId_(other.playerId_),
	playerName_(other.playerName_),
	playerType_(other.playerType_),
	playerColor_(other.playerColor_),
	modelName_(other.modelName_),
	destinationId_(other.destinationId_),
	playerTeam_(other.playerTeam_),
	playerIconName_(other.playerIconName_),
	spectate_(other.spectate_),
	tankType_(other.tankType_)
{
	playerIcon_.reset();
	playerIcon_.addDataToBuffer(other.playerIcon_.getBuffer(), 
		other.playerIcon_.getBufferUsed());
}

ComsTankChangeMessage::ComsTankChangeMessage(
		unsigned int playerId,
		const LangString &playerName,
		Vector playerColor,
		const char *tankType,
		const char *modelName,
		unsigned int destinationId,
		unsigned int playerTeam,
		const char *playerType,
		bool spectate) :
	ComsMessage(ComsTankChangeMessageType),
	playerId_(playerId),
	playerName_(playerName),
	playerType_(playerType),
	playerColor_(playerColor),
	tankType_(tankType),
	modelName_(modelName),
	destinationId_(destinationId),
	playerTeam_(playerTeam),
	spectate_(spectate)
{
}

ComsTankChangeMessage::~ComsTankChangeMessage()
{
}

bool ComsTankChangeMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerName_);
	buffer.addToBuffer(playerType_);
	buffer.addToBuffer(tankType_);
	buffer.addToBuffer(modelName_);
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(destinationId_);
	buffer.addToBuffer(playerTeam_);
	buffer.addToBuffer(playerColor_);
	buffer.addToBuffer(spectate_);
	buffer.addToBuffer(playerIcon_.getBufferUsed());
	if (playerIcon_.getBufferUsed() > 0)
	{
		buffer.addToBuffer(playerIconName_);
		buffer.addDataToBuffer(playerIcon_.getBuffer(), 
			playerIcon_.getBufferUsed());
	}
	return true;
}

bool ComsTankChangeMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerName_)) return false;
	if (!reader.getFromBuffer(playerType_)) return false;
	if (!reader.getFromBuffer(tankType_)) return false;
	if (!reader.getFromBuffer(modelName_)) return false;
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(destinationId_)) return false;
	if (!reader.getFromBuffer(playerTeam_)) return false;
	if (!reader.getFromBuffer(playerColor_)) return false;
	if (!reader.getFromBuffer(spectate_)) return false;
	unsigned int used = 0;
	if (!reader.getFromBuffer(used)) return false;
	if (used > 0)
	{
		if (!reader.getFromBuffer(playerIconName_)) return false;
		playerIcon_.allocate(used);
		playerIcon_.setBufferUsed(used);
		if (!reader.getDataFromBuffer(playerIcon_.getBuffer(), 
			used)) return false;
	}
	return true;
}
