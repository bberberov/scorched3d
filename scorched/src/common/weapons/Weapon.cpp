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

#include <weapons/Weapon.h>
#include <weapons/AccessoryStore.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/OptionsScorched.h>

WeaponFireContextInternal::WeaponFireContextInternal(unsigned int selectPositionX, unsigned int selectPositionY,
		const FixedVector &velocityVector, bool referenced, bool updateStats) :
	referenced_(referenced), updateStats_(updateStats),
	killCount_(0), referenceCount_(0), labelCount_(0),
	velocityVector_(velocityVector),
	selectPositionX_(selectPositionX), selectPositionY_(selectPositionY)
{
}

WeaponFireContextInternal::~WeaponFireContextInternal()
{
	delete labelCount_;
}

void WeaponFireContextInternal::incrementReference()
{
	referenceCount_++;
}

void WeaponFireContextInternal::decrementReference()
{
	referenceCount_--;
	if (referenceCount_ == 0) delete this;
}

int WeaponFireContextInternal::getIncLabelCount(unsigned int label)
{
	if (!labelCount_) labelCount_ = new std::map<unsigned int, int>();

	std::map<unsigned int, int>::iterator findItor =
		labelCount_->find(label);
	if (findItor == labelCount_->end()) (*labelCount_)[label] = 0;

	return ++(*labelCount_)[label];
}

WeaponFireContext::WeaponFireContext(unsigned int playerId, 
	unsigned int selectPositionX, unsigned int selectPositionY,
	const FixedVector &velocityVector,
	bool referenced, bool updateStats) :
	playerId_(playerId)
{
	internalContext_ = new WeaponFireContextInternal(selectPositionX, selectPositionY,
		velocityVector,
		referenced, updateStats);
	internalContext_->incrementReference();
}

WeaponFireContext::WeaponFireContext(WeaponFireContext &other) :
	playerId_(other.playerId_),
	internalContext_(other.internalContext_)
{
	internalContext_->incrementReference();
}

WeaponFireContext::~WeaponFireContext()
{
	internalContext_->decrementReference();
}

Weapon::Weapon() : 
	armsLevel_(-1)
{

}

Weapon::~Weapon()
{

}

bool Weapon::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	// Get the optional weapon armslevel
	accessoryNode->getNamedChild("armslevel", armsLevel_, false);

	return true;
}

int Weapon::getArmsLevel()
{
	if (armsLevel_ == -1) return parent_->getArmsLevel();
	return armsLevel_;
}

void Weapon::fire(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	if (context.getOptionsGame().getWeaponSyncCheck())
	{
		addWeaponSyncCheck(context, weaponContext, position, velocity);
	}

	fireWeapon(context, weaponContext, position, velocity);
}

void Weapon::addWeaponSyncCheck(ScorchedContext &context,
	WeaponFireContext &weaponContext,
	FixedVector &position, FixedVector &velocity)
{
	context.getSimulator().addSyncCheck(S3D::formatStringBuffer("WeaponFire %s-%u-%s %u %s %s",
		getParent()->getName(), getParent()->getAccessoryId(), getAccessoryTypeName(),
		weaponContext.getPlayerId(),
		position.asQuickString(), velocity.asQuickString()));
}