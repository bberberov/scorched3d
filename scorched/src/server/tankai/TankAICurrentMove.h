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

#if !defined(AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAICurrentTarget.h>
#include <tankai/TankAICurrentMoveWeapons.h>
#include <common/Vector.h>

class TankAICurrentMove
{
public:
	TankAICurrentMove();
	virtual ~TankAICurrentMove();

	virtual bool parseConfig(XMLNode *node);

	void clear();
	void playMove(Tanket *tanket, 
		TankAIWeaponSets::WeaponSet *weapons,
		bool useBatteries,
		unsigned int moveId);

	TankAICurrentTarget &getTargets() { return targets_; }

protected:
	struct ShotRecord
	{
		Vector position;
		float projectileCurrentDistance;
		float sniperCurrentDistance;
	};
	struct MoveData
	{
		unsigned int moveId;
		bool madeMove;
		bool usedBatteries;
	};

	std::map<Tanket *, ShotRecord> shotRecords_;
	TankAICurrentTarget targets_;
	float totalDamageBeforeMove_;
	bool useResign_, useFuel_;
	float movementDamage_, movementDamageChance_, movementLife_;
	float movementRandom_, movementCloseness_;
	float groupShotChance_, groupTargetDistance_;
	int groupShotSize_;
	float resignLife_;
	float largeWeaponUseDistance_;
	float sniperUseDistance_;
	float sniperStartDistance_, sniperEndDistance_;
	float sniperMinDecrement_, sniperMaxDecrement_;
	float sniperMovementFactor_;
	float projectileStartDistance_, projectileEndDistance_;
	float projectileMinDecrement_, projectileMaxDecrement_;
	float projectileMovementFactor_;
	float projectileMinDistance_;

	void playMoveInternal(Tanket *tanket, 
		TankAIWeaponSets::WeaponSet *weapons,
		bool useBatteries,
		MoveData &moveData);
	bool shootAtTank(Tanket *tanket, Tanket *targetTanket, 
		TankAICurrentMoveWeapons &weapons, MoveData &moveData);
	bool makeProjectileShot(Tanket *tanket, Tanket *targetTanket,
		TankAICurrentMoveWeapons &weapons, MoveData &moveData);
	bool makeSniperShot(Tanket *tanket, Tanket *targetTanket,
		TankAICurrentMoveWeapons &weapons, MoveData &moveData);
	bool makeLaserSniperShot(Tanket *tank, Tanket *targetTanket, 
		TankAICurrentMoveWeapons &weapons, MoveData &moveData);
	bool makeBurriedShot(Tanket *tanket, Tanket *targetTanket, 
		TankAICurrentMoveWeapons &weapons, MoveData &moveData);
	bool makeMoveShot(Tanket *tanket, 
		TankAIWeaponSets::WeaponSet *weapons,
		std::list<Tanket *> &sortedTankets,
		MoveData &moveData);
	bool makeGroupShot(Tanket *tanket, 
		TankAIWeaponSets::WeaponSet *weapons,
		std::list<Tanket *> &sortedTankets,
		MoveData &moveData);

	bool inHole(Vector &position);
	Vector lowestHighest(TankAICurrentMoveWeapons &weapons,
		Vector &position, bool highest);

	float getShotDistance(Tanket *tanket, bool projectile);
	void shotAtTank(Tanket *tanket, bool projectile, float newDistance);
	bool useAvailableBatteries(Tanket *tanket, MoveData &moveData);
	void setWeapon(Tanket *tanket, Accessory *accessory);
	void useBattery(Tanket *tanket, unsigned int batteryId, MoveData &moveData);
	void resign(Tanket *tanket, MoveData &moveData);
	void skipMove(Tanket *tanket, MoveData &moveData);
	void fireShot(Tanket *tanket, MoveData &moveData);

};

#endif // !defined(AFX_TankAICurrentMove_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
