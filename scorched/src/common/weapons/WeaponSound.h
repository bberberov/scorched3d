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

#if !defined(__INCLUDE_WeaponSoundh_INCLUDE__)
#define __INCLUDE_WeaponSoundh_INCLUDE__

#include <weapons/Weapon.h>
#include <vector>

class WeaponSound : public Weapon
{
public:
	WeaponSound();
	virtual ~WeaponSound();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	const char *getSound();
	fixed getGain() { return gain_; }
	fixed getRolloff() { return rolloff_; }
	fixed getReferenceDistance() { return referenceDistance_; }
	bool getRelative() { return relative_; }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponSound, AccessoryPart::AccessoryWeapon);
protected:
	std::vector<std::string> sounds_;
	fixed gain_;
	fixed rolloff_;
	fixed referenceDistance_;
	bool relative_;
};

#endif // __INCLUDE_WeaponSoundh_INCLUDE__
