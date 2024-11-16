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

#if !defined(__INCLUDE_ComsPlayedMoveMessageh_INCLUDE__)
#define __INCLUDE_ComsPlayedMoveMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsPlayedMoveMessage : public ComsMessage
{
public:
	static ComsMessageType ComsPlayedMoveMessageType;

	enum MoveType
	{
		eNone,
		eShot,
		eResign,
		eSkip,
		eFinishedBuy,
		eTimeout
	};

	ComsPlayedMoveMessage();
	ComsPlayedMoveMessage(const ComsPlayedMoveMessage &other);
	ComsPlayedMoveMessage(unsigned int playerId,
		unsigned int moveId,
		MoveType type);
	virtual ~ComsPlayedMoveMessage();

	void setShot(unsigned int weaponId,
		fixed rotationXY,
		fixed rotationYZ,
		fixed power,
		int positionX, 
		int positionY);

	unsigned int getPlayerId() { return playerId_; }
	unsigned int getMoveId() { return moveId_; }
	unsigned int getWeaponId() { return weaponId_; }
	fixed getRotationXY() { return rotationXY_; }
	fixed getRotationYZ() { return rotationYZ_; }
	int getSelectPositionX() { return selectPositionX_; }
	int getSelectPositionY() { return selectPositionY_; }
	fixed getPower() { return power_; }
	MoveType getType() { return moveType_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	unsigned int moveId_;
	unsigned int weaponId_;
	MoveType moveType_;
	fixed rotationXY_;
	fixed rotationYZ_;
	fixed power_;
	int selectPositionX_;
	int selectPositionY_;

private:
	const ComsPlayedMoveMessage & operator=(const ComsPlayedMoveMessage &);
};

#endif
