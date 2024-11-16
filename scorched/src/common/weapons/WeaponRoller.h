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

#if !defined(__INCLUDE_WeaponRollerh_INCLUDE__)
#define __INCLUDE_WeaponRollerh_INCLUDE__

#include <weapons/Weapon.h>
#include <common/ModelID.h>
#include <engine/ObjectGroupEntryDefinition.h>

class ScorchedContext;
class WeaponRoller : public Weapon
{
public:
	WeaponRoller();
	virtual ~WeaponRoller();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	Weapon *getCollisionAction() { return collisionAction_; }
	ModelID &getRollerModelID() { return rollerModelId_; }
	bool getRoll() { return roll_; }
	bool getStickyShields() { return stickyShields_; }
	bool getNoCameraTrack() { return noCameraTrack_; }
	bool getLandscapeCollision() { return landscapeCollision_; }
	bool getShieldCollision() { return shieldCollision_; }
	bool getTankCollision() { return tankCollision_; }
	bool getTargetCollision() { return targetCollision_; }
	fixed getShieldHurtFactor(ScorchedContext &context) { return shieldHurtFactorExp_.getValue(context); }
	fixed getTime(ScorchedContext &context) { return timeExp_.getValue(context); } 
	fixed getWindFactor(ScorchedContext &context) { return windFactorExp_.getValue(context); }
	fixed getGravityFactor(ScorchedContext &context) { return gravityFactorExp_.getValue(context); }
	fixed getStepSize() { return stepSize_; }
	fixed getScale(ScorchedContext &context) { return scale_.getValue(context); }
	fixed getTimeout(ScorchedContext &context) { return timeout_.getValue(context); }

	ObjectGroupEntryDefinition &getLocalGroups() { return localGroups_; }
	ObjectGroupEntryDefinition &getGlobalGroups() { return globalGroups_; }

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponRoller, AccessoryPart::AccessoryWeapon);

protected:
	NumberParser numberRollers_;
	Weapon *collisionAction_;
	ModelID rollerModelId_;
	ObjectGroupEntryDefinition localGroups_, globalGroups_;
	NumberParser dampenVelocityExp_;
	NumberParser shieldHurtFactorExp_;
	NumberParser gravityFactorExp_;
	NumberParser windFactorExp_;
	NumberParser timeExp_;
	NumberParser scale_;
	NumberParser timeout_;
	fixed stepSize_;
	bool roll_;
	bool stickyShields_;
	bool landscapeCollision_;
	bool shieldCollision_;
	bool tankCollision_;
	bool targetCollision_;
	bool maintainVelocity_;
	bool noCameraTrack_;
	
	void addRoller(ScorchedContext &context, 
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);

};

#endif
