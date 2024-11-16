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

#include <target/TargetShield.h>
#include <target/TargetSpace.h>
#include <target/TargetContainer.h>
#include <weapons/AccessoryStore.h>
#include <weapons/ShieldRound.h>
#include <weapons/ShieldSquare.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <common/OptionsScorched.h>

TargetShield::TargetShield(ScorchedContext &context,
	unsigned int playerId) :
	context_(context),
	currentShield_(0),
	power_(0), 
	target_(0),
	boundingSize_(0),
	graphicalCurrentShield_(0),
	graphicalShieldPower_(0)
{
}

TargetShield::~TargetShield()
{
}

void TargetShield::loaded()
{
	setCurrentShield(0);
}

void TargetShield::setCurrentShield(Accessory *sh)
{
	if (sh)
	{
		Shield *shield = (Shield *) sh->getAction();
		power_ = shield->getPower();
		currentShield_ = sh;	
		boundingSize_ = shield->getBoundingSize();
	}
	else
	{
		power_ = 0;
		currentShield_ = 0;
		boundingSize_ = 0;
	}

	// Update the target space with this new shield information
	context_.getTargetSpace().updateTarget(target_);
}

void TargetShield::setShieldPower(fixed power)
{
	power_ = power;
	if (power_ <= 0)
	{
		power_ = 0;
		setCurrentShield(0);
	}
}

bool TargetShield::returnGraphical()
{
	if (target_->getType() != Target::TypeTank) return false;
	if (!context_.getOptionsGame().getDelayedDefenseActivation()) return false;
	Tank *thisTank = (Tank *) target_;
	Tank *currentTank = context_.getTargetContainer().getCurrentTank();
	if (thisTank == currentTank) return false;
	if (!currentTank) return false;

	return true;
}

Accessory *TargetShield::getGraphicalCurrentShield()
{
	if (returnGraphical())
	{
		return graphicalCurrentShield_;
	}
	else
	{
		graphicalCurrentShield_ = currentShield_;
		return currentShield_;
	}
}

fixed TargetShield::getGraphicalShieldPower()
{
	if (returnGraphical())
	{
		return graphicalShieldPower_;
	}
	else
	{
		graphicalShieldPower_ = power_;
		return power_;
	}
}

bool TargetShield::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TargetShield");

	unsigned int shieldId = (currentShield_?currentShield_->getAccessoryId():0);
	buffer.addToBufferNamed("id", shieldId);
	if (shieldId != 0) buffer.addToBufferNamed("power", power_);
	return true;
}

bool TargetShield::readMessage(NetBufferReader &reader)
{
	unsigned int shieldId;
	if (!reader.getFromBuffer(shieldId)) return false;
	if (shieldId == 0) setCurrentShield(0);
	else setCurrentShield(context_.getAccessoryStore().findByAccessoryId(shieldId));
	if (shieldId != 0) if (!reader.getFromBuffer(power_)) return false;
	return true;
}
