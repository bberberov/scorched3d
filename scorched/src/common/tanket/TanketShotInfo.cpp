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

#include <tanket/Tanket.h>
#include <tank/TankLib.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketType.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

TanketShotInfo::TanketShotInfo(ScorchedContext &context) :
	turretRotXY_(0), turretRotYZ_(0),
	power_(1000), maxPower_(1000),
	tanket_(0), context_(context),
	selectPositionX_(0), selectPositionY_(0),
	moveId_(0), missedMoves_(0), skippedShots_(0),
	useNormalMoves_(true)
{
	turretRotXY_ = 0;
	turretRotYZ_ = 45;
}

TanketShotInfo::~TanketShotInfo()
{
}

void TanketShotInfo::newGame()
{
	TanketType *type = tanket_->getTanketType();
	maxPower_ = power_ = type->getPower();
	missedMoves_ = 0;
	skippedShots_ = 0;
	if (!context_.getServerMode()) moveId_ = 0;
}

fixed TanketShotInfo::rotateGunXY(fixed angle, bool diff)
{
	if (diff) turretRotXY_ += angle;
	else turretRotXY_ = angle;

	if (turretRotXY_ <= 0) turretRotXY_ = turretRotXY_ + 360;
	else if (turretRotXY_ > 360) turretRotXY_ = turretRotXY_ - 360;

	return turretRotXY_;
}

fixed TanketShotInfo::rotateGunYZ(fixed angle, bool diff)
{
	if (diff) turretRotYZ_ += angle;
	else turretRotYZ_ = angle;

	if (turretRotYZ_ < 0) turretRotYZ_ = 0;
	else if (turretRotYZ_ > 90) turretRotYZ_ = 90;

	return turretRotYZ_;
}

fixed TanketShotInfo::changePower(fixed power, bool diff)
{
	if (diff) power_ += power;
	else power_ = power;

	if (power_ < 0) power_ = 0;
	if (context_.getOptionsGame().getLimitPowerByHealth())
	{
		fixed maxPosPower = 
			tanket_->getLife().getLife() / tanket_->getLife().getMaxLife() * maxPower_;
		if (power_ > maxPosPower) power_ = maxPosPower;
	}
	if (power_ > maxPower_) power_ = maxPower_;

	return power_;
}

bool TanketShotInfo::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TanketShotInfo");

	buffer.addToBufferNamed("maxPower", maxPower_);
	return true;
}

bool TanketShotInfo::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(maxPower_)) return false;
	return true;
}
