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

#include <actions/AddTarget.h>
#include <engine/ActionController.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <target/Target.h>
#include <target/TargetContainer.h>
#include <target/TargetDamageCalc.h>
#include <tankai/TankAIAdder.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponAddTarget.h>
#include <weapons/Shield.h>
#include <common/RandomGenerator.h>
#include <common/OptionsScorched.h>

AddTarget::AddTarget(FixedVector &position,
	WeaponAddTarget *addTarget) :
	Action(true),
	position_(position),
	addTarget_(addTarget)
{

}

AddTarget::~AddTarget()
{
}

void AddTarget::init()
{
}

void AddTarget::simulate(fixed frameTime, bool &remove)
{
	unsigned int playerId = TankAIAdder::getNextTargetId(*context_);

	Target *target = addTarget_->getTargetDefinition().createTarget(
		playerId, position_, FixedVector::getNullVector(), *context_, 
		context_->getSimulator().getRandomGenerator());

	if (context_->getOptionsGame().getActionSyncCheck())
	{
		context_->getSimulator().addSyncCheck(
			S3D::formatStringBuffer("AddTarget : %u %s %s", 
				playerId,
				target->getCStrName().c_str(),
				position_.asQuickString()));
	}

	// Check if this new target can fall
	WeaponFireContext weaponContext(playerId, 0, 0, FixedVector(), false, false);
	TargetDamageCalc::damageTarget(*context_, target->getPlayerId(), addTarget_, 
		weaponContext, 0, false, true, false);

	remove = true;
	Action::simulate(frameTime, remove);
}

std::string AddTarget::getActionDetails()
{
	return addTarget_->getParent()->getName();
}
