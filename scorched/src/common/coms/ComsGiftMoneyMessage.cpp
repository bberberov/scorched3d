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

#include <coms/ComsGiftMoneyMessage.h>

ComsMessageType ComsGiftMoneyMessage::ComsGiftMoneyMessageType("ComsGiftMoneyMessageType");

ComsGiftMoneyMessage::ComsGiftMoneyMessage() :
	ComsMessage(ComsGiftMoneyMessageType),
	fromPlayerId_(0), 
	toPlayerId_(0),
	money_(0)
{
}

ComsGiftMoneyMessage::ComsGiftMoneyMessage(
	unsigned int fromPlayerId,
	unsigned int toPlayerId,
	int money) :
	ComsMessage(ComsGiftMoneyMessageType),
	fromPlayerId_(fromPlayerId), 
	toPlayerId_(toPlayerId),
	money_(money)
{
}

ComsGiftMoneyMessage::ComsGiftMoneyMessage(
	ComsGiftMoneyMessage &other) :
	ComsMessage(ComsGiftMoneyMessageType),
	fromPlayerId_(other.fromPlayerId_), 
	toPlayerId_(other.toPlayerId_),
	money_(other.money_)
{
}

ComsGiftMoneyMessage::~ComsGiftMoneyMessage()
{
}

bool ComsGiftMoneyMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(fromPlayerId_);
	buffer.addToBuffer(toPlayerId_);
	buffer.addToBuffer(money_);
	return true;
}

bool ComsGiftMoneyMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(fromPlayerId_)) return false;
	if (!reader.getFromBuffer(toPlayerId_)) return false;
	if (!reader.getFromBuffer(money_)) return false;
	return true;
}
