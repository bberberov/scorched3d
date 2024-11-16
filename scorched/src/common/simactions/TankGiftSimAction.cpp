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

#include <simactions/TankGiftSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <lang/LangResource.h>
#include <common/ChannelManager.h>
#ifndef S3D_SERVER
#include <dialogs/BuyAccessoryDialog.h>
#endif

REGISTER_CLASS_SOURCE(TankGiftSimAction);

TankGiftSimAction::TankGiftSimAction()
{
}

TankGiftSimAction::TankGiftSimAction(ComsGiftMoneyMessage &giftMessage) :
	giftMessage_(giftMessage)
{
}

TankGiftSimAction::~TankGiftSimAction()
{
}

bool TankGiftSimAction::invokeAction(ScorchedContext &context)
{
	unsigned int fromPlayerId = giftMessage_.getFromPlayerId();
	unsigned int toPlayerId = giftMessage_.getToPlayerId();
	int money = giftMessage_.getMoney();

	// Check tank exists and is alive
	Tank *fromTank = context.getTargetContainer().getTankById(fromPlayerId);
	if (!fromTank || !fromTank->getState().getTankPlaying())
	{
		return true;
	}

	// Check to player
	Tank *toTank = context.getTargetContainer().getTankById(toPlayerId);
	if (!toTank || !toTank->getState().getTankPlaying())
	{
		return true;
	}

	// Check teams
	if (toTank->getTeam() != fromTank->getTeam())
	{
		return true;
	}
	if (toTank == fromTank)
	{
		return true;
	}

	// Check tank has required amount of money
	if (money < 0) return true;
	if (fromTank->getScore().getMoney() < money) return true;

	// Perform action
	fromTank->getScore().setMoney(
		fromTank->getScore().getMoney() - money);
	toTank->getScore().setMoney(
		toTank->getScore().getMoney() + money);

#ifndef S3D_SERVER
	if (!context.getServerMode()) 
	{
		if (BuyAccessoryDialogTankInfo::instance()->tankId == fromTank->getPlayerId())
		{
			BuyAccessoryDialogTankInfo::instance()->tankMoney -= money;
		}
		if (BuyAccessoryDialogTankInfo::instance()->tankId == toTank->getPlayerId())
		{
			BuyAccessoryDialogTankInfo::instance()->tankMoney += money;
		}
		BuyAccessoryDialog::instance()->playerRefreshKeepPos();
	}
#endif

	// Log transaction
	ChannelText text("combat", 
		LANG_RESOURCE_3(
			"TANK_GIFT_MESSAGE", 
			"[p:{0}] gifts {1} to [p:{2}]", 
			fromTank->getTargetName(), 
			S3D::formatMoney(money), 
			toTank->getTargetName()));
	ChannelManager::showText(context, text);

	return true;
}

bool TankGiftSimAction::writeMessage(NetBuffer &buffer)
{
	return giftMessage_.writeMessage(buffer);
}

bool TankGiftSimAction::readMessage(NetBufferReader &reader)
{
	return giftMessage_.readMessage(reader);
}
