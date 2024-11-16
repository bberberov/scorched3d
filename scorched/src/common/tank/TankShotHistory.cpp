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

#include <tank/Tank.h>
#include <tank/TankLib.h>
#include <tank/TankShotHistory.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

TankShotHistory::TankShotHistory(ScorchedContext &context) :
	oldTurretRotXY_(0), oldTurretRotYZ_(0), oldPower_(1000),
	tank_(0), context_(context)
{
}

TankShotHistory::~TankShotHistory()
{
}

std::vector<TankShotHistory::ShotEntry> &TankShotHistory::getOldShots()
{
	std::vector<ShotEntry>::iterator itor;
	for (itor = oldShots_.begin();
		itor != oldShots_.end();
		++itor)
	{
		ShotEntry &entry = *itor;
		entry.current = 
			(entry.ele == oldTurretRotYZ_ &&
			entry.rot == oldTurretRotXY_ &&
			entry.power == oldPower_);
	}

	return oldShots_; 
}

void TankShotHistory::clientNewGame()
{
	oldShots_.clear();
	madeShot();
}

void TankShotHistory::madeShot()
{
	oldPower_ = tank_->getShotInfo().getPower();
	oldTurretRotXY_ = tank_->getShotInfo().getRotationGunXY();
	oldTurretRotYZ_ = tank_->getShotInfo().getRotationGunYZ();

	if (oldShots_.empty() ||
		oldShots_.back().power != tank_->getShotInfo().getPower() ||
		oldShots_.back().rot != tank_->getShotInfo().getRotationGunXY() ||
		oldShots_.back().ele != tank_->getShotInfo().getRotationGunYZ())
	{
		oldShots_.push_back(ShotEntry(
			tank_->getShotInfo().getPower(), 
			tank_->getShotInfo().getRotationGunXY(), 
			tank_->getShotInfo().getRotationGunYZ()));
	}

	if (oldShots_.size() > 15) 
	{
		oldShots_.erase(oldShots_.begin());
	}
}

void TankShotHistory::revertSettings(unsigned int index)
{
	if (index < oldShots_.size())
	{
		int newIndex = (int) (oldShots_.size() - 1) - index;
		tank_->getShotInfo().rotateGunXY(oldShots_[newIndex].rot, false);
		tank_->getShotInfo().rotateGunYZ(oldShots_[newIndex].ele, false);
		tank_->getShotInfo().changePower(oldShots_[newIndex].power, false);

		oldPower_ = tank_->getShotInfo().getPower();
		oldTurretRotXY_ = tank_->getShotInfo().getRotationGunXY();
		oldTurretRotYZ_ = tank_->getShotInfo().getRotationGunYZ();
	}
}

void TankShotHistory::undo()
{
	tank_->getShotInfo().rotateGunXY(oldTurretRotXY_, false);
	tank_->getShotInfo().rotateGunYZ(oldTurretRotYZ_, false);
	tank_->getShotInfo().changePower(oldPower_, false);
}

fixed TankShotHistory::getRotationXYDiff()
{
	fixed rotDiff = (fixed(360) - tank_->getShotInfo().getRotationGunXY()) - (fixed(360) - oldTurretRotXY_);
	if (rotDiff > 180) rotDiff -= 360;
	else if (rotDiff < -180) rotDiff += 360;
	return rotDiff;
}

fixed TankShotHistory::getRotationYZDiff()
{
	return tank_->getShotInfo().getRotationGunYZ() - oldTurretRotYZ_;
}

fixed TankShotHistory::getPowerDiff()
{
	return tank_->getShotInfo().getPower() - oldPower_;
}

const char *TankShotHistory::getRotationString()
{
	static char messageBuffer[255];
	fixed rotDiff = getRotationXYDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		(fixed(360) - tank_->getShotInfo().getRotationGunXY()).asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

const char *TankShotHistory::getElevationString()
{
	static char messageBuffer[255];
	fixed rotDiff = getRotationYZDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 
		tank_->getShotInfo().getRotationGunYZ().asFloat(),
		rotDiff.asFloat());
	return messageBuffer;
}

const char *TankShotHistory::getPowerString()
{
	static char messageBuffer[255];
	fixed powDiff = getPowerDiff();

	snprintf(messageBuffer, 255, "%+.1f (%+.1f)", 		
		tank_->getShotInfo().getPower().asFloat(),
		powDiff.asFloat());
	return messageBuffer;
}
