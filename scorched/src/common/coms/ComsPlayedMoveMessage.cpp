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

#include <coms/ComsPlayedMoveMessage.h>

ComsMessageType ComsPlayedMoveMessage::ComsPlayedMoveMessageType("ComsPlayedMoveMessageType");

ComsPlayedMoveMessage::ComsPlayedMoveMessage() :
	ComsMessage(ComsPlayedMoveMessageType),
	moveType_(eNone),
	weaponId_(0),
	moveId_(0),
	rotationXY_(0), rotationYZ_(0), power_(0), playerId_(0),
	selectPositionX_(0), selectPositionY_(0)
{
}

ComsPlayedMoveMessage::ComsPlayedMoveMessage(const ComsPlayedMoveMessage &other) :
	ComsMessage(ComsPlayedMoveMessageType),
	moveType_(other.moveType_),
	weaponId_(other.weaponId_),
	moveId_(other.moveId_),
	rotationXY_(other.rotationXY_), rotationYZ_(other.rotationYZ_), 
	power_(other.power_), playerId_(other.playerId_),
	selectPositionX_(other.selectPositionX_), selectPositionY_(other.selectPositionY_)
{
}

ComsPlayedMoveMessage::ComsPlayedMoveMessage(unsigned int playerId,
	unsigned int moveId,
	MoveType type) :
	ComsMessage(ComsPlayedMoveMessageType),
	moveType_(type),
	weaponId_(0),
	moveId_(moveId),
	rotationXY_(0), rotationYZ_(0), power_(0), playerId_(playerId),
	selectPositionX_(0), selectPositionY_(0)
{

}

ComsPlayedMoveMessage::~ComsPlayedMoveMessage()
{
}

void ComsPlayedMoveMessage::setShot(unsigned int weaponId,
	fixed rotationXY,
	fixed rotationYZ,
	fixed power,
	int selectPositionX, 
	int selectPositionY)
{
	weaponId_ = weaponId;
	rotationXY_ = rotationXY;
	rotationYZ_ = rotationYZ;
	power_ = power;
	selectPositionX_ = selectPositionX;
	selectPositionY_ = selectPositionY;
}

bool ComsPlayedMoveMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(moveId_);
	buffer.addToBuffer((int) moveType_);
	if (moveType_ == eShot)
	{
		buffer.addToBuffer(weaponId_);
		buffer.addToBuffer(rotationXY_);
		buffer.addToBuffer(rotationYZ_);
		buffer.addToBuffer(power_);
		buffer.addToBuffer(selectPositionX_);
		buffer.addToBuffer(selectPositionY_);
	}
	return true;
}

bool ComsPlayedMoveMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(moveId_)) return false;
	int mt;
	if (!reader.getFromBuffer(mt)) return false;
	moveType_ = (MoveType) mt;
	if (moveType_ == eShot)
	{
		if (!reader.getFromBuffer(weaponId_)) return false;
		if (!reader.getFromBuffer(rotationXY_)) return false;
		if (!reader.getFromBuffer(rotationYZ_)) return false;
		if (!reader.getFromBuffer(power_)) return false;
		if (!reader.getFromBuffer(selectPositionX_)) return false;
		if (!reader.getFromBuffer(selectPositionY_)) return false;
	}
	return true;
}

