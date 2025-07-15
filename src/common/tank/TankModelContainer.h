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

#ifndef _TankModelContainer_h
#define _TankModelContainer_h

#include <net/NetBuffer.h>

// The model used for this tank
class ScorchedContext;
class TankModel;
class Tank;
class TankModelContainer
{
public:
	TankModelContainer(ScorchedContext &context);
	virtual ~TankModelContainer();

	void setTank(Tank *tank) { tank_ = tank; }

	TankModel *getTankModel();

	void setTankModelName(const char *modelName);

	// Serialize the modelid
    bool writeMessage(NamedNetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	ScorchedContext &context_;
	Tank *tank_;
	std::string modelName_;
	TankModel *tankModel_;
};

#endif // _TankModelContainer_h
