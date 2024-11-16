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

#if !defined(__INCLUDE_TargetShieldh_INCLUDE__)
#define __INCLUDE_TargetShieldh_INCLUDE__

#include <engine/ScorchedCollisionIds.h>
#include <net/NetBuffer.h>

class Target;
class Accessory;
class ScorchedContext;
class TargetShield
{
public:
	TargetShield(ScorchedContext &context,
		unsigned int playerId);
	virtual ~TargetShield();

	virtual void loaded();
	void setTarget(Target *target) { target_ = target; }

	Accessory *getCurrentShield() { return currentShield_; }
	void setCurrentShield(Accessory *sh);

	// Used only for delayed shield defense
	Accessory *getGraphicalCurrentShield();
	fixed getGraphicalShieldPower();

	fixed getShieldPower() { return power_; }
	fixed getShieldBoundingSize() { return boundingSize_; }
	void setShieldPower(fixed power);

	// Serialize
    bool writeMessage(NamedNetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	Target *target_;
	ScorchedContext &context_;
	Accessory *currentShield_;
	fixed power_, boundingSize_;
	Accessory *graphicalCurrentShield_;
	fixed graphicalShieldPower_;

	bool returnGraphical();
};

#endif // __INCLUDE_TargetShieldh_INCLUDE__
