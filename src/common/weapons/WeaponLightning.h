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

#if !defined(__INCLUDE_WeaponLightningh_INCLUDE__)
#define __INCLUDE_WeaponLightningh_INCLUDE__

#include <weapons/Weapon.h>

class WeaponLightning : public Weapon
{
public:
	WeaponLightning();
	virtual ~WeaponLightning();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	fixed getConeLength() { return coneLength_; }
	fixed getSegLength() { return segLength_; }
	fixed getSegVar() { return segVar_; }
	fixed getSize() { return size_; }
	fixed getSizeVar() { return sizeVar_; }
	fixed getMinSize() { return minSize_; }
	fixed getSplitProb() { return splitProb_; }
	fixed getSplitVar() { return splitVar_; }
	fixed getDeathProb() { return deathProb_; }
	fixed getDerivAngle() { return derivAngle_; }
	fixed getAngleVar() { return angleVar_; }
	fixed getTotalTime() { return totalTime_; }
	fixed getSegHurt() { return segHurt_; }
	fixed getSegHurtRadius() { return segHurtRadius_; }
	const char *getSound() { return sound_.c_str(); }
	const char *getTexture() { return texture_.c_str(); }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponLightning, AccessoryPart::AccessoryWeapon);

protected:
	fixed coneLength_;
	fixed segLength_;
	fixed segVar_;
	fixed size_;
	fixed sizeVar_;
	fixed minSize_;
	fixed splitProb_;
	fixed splitVar_;
	fixed deathProb_;
	fixed derivAngle_;
	fixed angleVar_;
	fixed totalTime_;
	fixed segHurt_;
	fixed segHurtRadius_;
	std::string sound_;
	std::string texture_;
	NumberParser coneLengthExp_;
	NumberParser segLengthExp_;
	NumberParser segVarExp_;
	NumberParser sizeExp_;
	NumberParser sizeVarExp_;
	NumberParser minSizeExp_;
	NumberParser splitProbExp_;
	NumberParser splitVarExp_;
	NumberParser deathProbExp_;
	NumberParser derivAngleExp_;
	NumberParser angleVarExp_;
	NumberParser totalTimeExp_;
	NumberParser segHurtExp_;
	NumberParser segHurtRadiusExp_;

};

#endif // __INCLUDE_WeaponLightningh_INCLUDE__
