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

#include <dialogs/BuyAccessoryDialogTankInfo.hpp>
#include <client/ScorchedClient.hpp>
#include <target/TargetContainer.hpp>
#include <tank/Tank.hpp>
#include <tank/TankScore.hpp>
#include <tank/TankState.hpp>
#include <tanket/TanketShotInfo.hpp>

BuyAccessoryDialogTankInfo *BuyAccessoryDialogTankInfo::instance_ = nullptr;

BuyAccessoryDialogTankInfo *BuyAccessoryDialogTankInfo::instance()
{
	if (nullptr == instance_)
	{
		instance_ = new BuyAccessoryDialogTankInfo();
	}
	return instance_;
}

BuyAccessoryDialogTankInfo::BuyAccessoryDialogTankInfo() :
	tankAccessories(ScorchedClient::instance()->getContext())
{
}

void BuyAccessoryDialogTankInfo::set()
{
	Tank *tank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (tank)
	{
		tankAccessories.setTanket(tank);
		NetBuffer buffer;
		tank->getAccessories().writeMessage(buffer, true);
		NetBufferReader reader(buffer);
		tankAccessories.readMessage(reader);
		tankColor = tank->getColor();
		tankId = tank->getPlayerId();
		tankMoney = tank->getScore().getMoney();
		tankName = tank->getTargetName();
		tankMoveId = tank->getShotInfo().getMoveId();
	} 
	else
	{
		tankAccessories.setTanket( nullptr );
		tankAccessories.clearAccessories();
		tankColor = Vector::getNullVector();
		tankName = LangString();
		tankMoney = 0;
		tankId = 0;
		tankMoveId = 0;
	}
}
