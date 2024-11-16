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

#if !defined(AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_)
#define AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_

#include <weapons/AccessoryPart.h>
#include <net/NetBuffer.h>
#include <common/FixedVector.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroups.h>

class WeaponFireContextInternal
{
public:
	WeaponFireContextInternal(unsigned int selectPositionX, unsigned int selectPositionY,
		const FixedVector &velocityVectory,
		bool referenced, bool updateStats);
	virtual ~WeaponFireContextInternal();

	int getKillCount() { return killCount_; }
	void setKillCount(int killCount) { killCount_ = killCount; }

	bool getUpdateStats() { return updateStats_; }
	bool getReferenced() { return referenced_; }

	unsigned int getSelectPositionX() { return selectPositionX_; }
	unsigned int getSelectPositionY() { return selectPositionY_; }
	FixedVector &getVelocityVector() { return velocityVector_; }

	ObjectGroups &getLocalGroups() { return localGroups_; }

	int getIncLabelCount(unsigned int label);

	void incrementReference();
	void decrementReference();

protected:
	int killCount_;
	bool referenced_, updateStats_;
	unsigned int referenceCount_;
	ObjectGroups localGroups_;
	unsigned int selectPositionX_;
	unsigned int selectPositionY_;
	FixedVector velocityVector_;
	std::map<unsigned int, int> *labelCount_;

private:
	WeaponFireContextInternal(WeaponFireContextInternal &other);
	WeaponFireContextInternal &operator=(WeaponFireContextInternal &other);
};

class WeaponFireContext
{
public:
	WeaponFireContext(unsigned int playerId, 
		unsigned int selectPositionX, unsigned int selectPositionY,
		const FixedVector &velocityVector,
		bool referenced, bool updateStats);
	WeaponFireContext(WeaponFireContext &other);
	virtual ~WeaponFireContext();

	unsigned int getPlayerId() { return playerId_; }
	void setPlayerId(unsigned int playerId) { playerId_ = playerId; }
	
	WeaponFireContextInternal &getInternalContext() { return *internalContext_; }

protected:
	unsigned int playerId_;
	WeaponFireContextInternal *internalContext_;

private:
	WeaponFireContext &operator=(WeaponFireContext &other);
	
};

class Action;
class Weapon : public AccessoryPart
{
public:
	Weapon();
	virtual ~Weapon();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);
	
	virtual void fire(ScorchedContext &context,
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);

	int getArmsLevel();

protected:
	int armsLevel_;

	virtual void addWeaponSyncCheck(ScorchedContext &context,
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity);
	virtual void fireWeapon(ScorchedContext &context,
		WeaponFireContext &weaponContext,
		FixedVector &position, FixedVector &velocity) = 0;
};

#endif // !defined(AFX_WEAPON_H__65439E20_84A6_406A_8FD0_045A3E7555D3__INCLUDED_)
