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

#include <math.h>
#include <tanket/Tanket.h>
#include <tanket/TanketType.h>
#include <tanket/TanketTypes.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
#include <tankai/TankAI.h>
#include <tankai/TankAIStore.h>
#include <target/TargetLife.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <common/Logger.h>

Tanket::Tanket(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const LangString &name) :
	Target(playerId, name, context), 
	context_(context),
	tankAI_(0), team_(0),
	destinationId_(destinationId)
{
	tanketType_ = context.getTanketTypes().getDefaultType();

	accessories_ = new TanketAccessories(context);
	accessories_->setTanket(this);
	shotInfo_ = new TanketShotInfo(context);
	shotInfo_->setTanket(this);
}

Tanket::~Tanket()
{
	delete accessories_; accessories_ = 0;
	delete shotInfo_; shotInfo_ = 0;
	delete tankAI_; tankAI_ = 0;
}

void Tanket::setTankAI(TankAI *ai)
{
	if (tankAI_) delete tankAI_;
	tankAI_ = ai;
}

unsigned int Tanket::getDestinationId() 
{ 
	if (getTankAI()) return 0;
	return destinationId_; 
}

void Tanket::newMatch()
{
	accessories_->newMatch();
}

void Tanket::newGame()
{
	getLife().setMaxLife(tanketType_->getLife());

	Target::newGame();

	if (tankAI_) tankAI_->newGame();
	shotInfo_->newGame();
}

bool Tanket::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "Tanket");

	buffer.addToBufferNamed("tankettype", tanketType_->getName());
	if (!Target::writeMessage(buffer)) return false;  // Base class 1st
	buffer.addToBufferNamed("destinationId", destinationId_);
	if (!accessories_->writeMessage(buffer, true)) return false;
	if (!shotInfo_->writeMessage(buffer)) return false;
	buffer.addToBufferNamed("team", team_);

	return true;
}

bool Tanket::readMessage(NetBufferReader &reader)
{
	std::string tanketTypeName;
	if (!reader.getFromBuffer(tanketTypeName))
	{
		Logger::log("Tanket::tanketType_ read failed");
		return false;
	}
	if (0 != strcmp(tanketTypeName.c_str(), tanketType_->getName()))
	{
		tanketType_ = context_.getTanketTypes().getType(tanketTypeName.c_str());
		if (!tanketType_) tanketType_ = context_.getTanketTypes().getDefaultType();
	}
	if (!Target::readMessage(reader)) 
	{
		Logger::log("Target::readMessage failed");
		return false; // Base class 1st
	}
	if (!reader.getFromBuffer(destinationId_))
	{
		Logger::log("Tank::destinationId_ read failed");
		return false;
	}
	if (!accessories_->readMessage(reader))
	{
		Logger::log("Tanket::accessories_ read failed");
		return false;
	}
	if (!shotInfo_->readMessage(reader))
	{
		Logger::log("Tanket::shotInfo_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(team_))
	{
		Logger::log("Tanket::team_ read failed");
		return false;
	}

	return true;
}
