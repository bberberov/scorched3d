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

#if !defined(__INCLUDE_TankShotHistoryh_INCLUDE__)
#define __INCLUDE_TankShotHistoryh_INCLUDE__

#include <net/NetBuffer.h>
#include <vector>

class Tank;
class ScorchedContext;
class TankShotHistory
{
public:
	struct ShotEntry
	{
		ShotEntry(fixed p = 0, fixed r = 0, fixed e = 0) : 
			power(p), rot(r), ele(e) { }
	
		fixed power;
		fixed rot;
		fixed ele;
		bool current;
	};

	TankShotHistory(ScorchedContext &context);
	virtual ~TankShotHistory();

	void setTank(Tank *tank) { tank_ = tank; }

	// State change
	void clientNewGame();
	void madeShot();
	
	// Saved settings
	fixed getRotationXYDiff();
	fixed getRotationYZDiff();
	fixed getPowerDiff();
	void revertSettings(unsigned int index = 0);
	void undo();
	std::vector<ShotEntry> &getOldShots();

	const char *getRotationString();
	const char *getElevationString();
	const char *getPowerString();

protected:
	ScorchedContext &context_;
	Tank *tank_;

	// Turret angles
	std::vector<ShotEntry> oldShots_;
	fixed oldTurretRotXY_, oldTurretRotYZ_, oldPower_;
};

#endif

