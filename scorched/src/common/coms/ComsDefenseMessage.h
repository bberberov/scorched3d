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


#if !defined(__INCLUDE_ComsDefenseMessageh_INCLUDE__)
#define __INCLUDE_ComsDefenseMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsDefenseMessage : public ComsMessage
{
public:
	static ComsMessageType ComsDefenseMessageType;

	enum DefenseChange
	{
		eNoChange,
		eShieldUp,
		eShieldDown,
		eParachutesUp,
		eParachutesDown,
		eBatteryUse
	};

	ComsDefenseMessage();
	ComsDefenseMessage(unsigned int playerId,
		DefenseChange change,
		unsigned int infoId);
	ComsDefenseMessage(ComsDefenseMessage &other);
	virtual ~ComsDefenseMessage();

	void setPlayerId(unsigned int id) { playerId_ = id; }
	void setInfoId(unsigned int infoId) { infoId_ = infoId; }
	unsigned int getPlayerId() { return playerId_; }
	DefenseChange getChange() { return change_; }
	unsigned int getInfoId() { return infoId_; }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	unsigned int infoId_;
	DefenseChange change_;

private:
	const ComsDefenseMessage & operator=(const ComsDefenseMessage &);
};


#endif
