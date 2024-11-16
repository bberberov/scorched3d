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

#if !defined(AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
#define AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_

#include <weapons/AccessoryPart.h>
#include <common/Vector.h>

class Shield : public AccessoryPart
{
public:
	enum ShieldType
	{
		ShieldTypeRoundNormal,
		ShieldTypeRoundReflective,
		ShieldTypeRoundMag,
		ShieldTypeSquareNormal,
		ShieldTypeSquareReflective
	};
	enum ShieldMovementType
	{
		ShieldMovementAll,
		ShieldMovementNone,
		ShieldMovementSame,
		ShieldMovementTeam1,
		ShieldMovementTeam2,
		ShieldMovementTeam3,
		ShieldMovementTeam4
	};
	enum ShieldLaserProofType
	{
		ShieldLaserProofNone,
		ShieldLaserProofStop,
		ShieldLaserProofTotal
	};

	Shield();
	virtual ~Shield();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// Shield attributes
	const char *getCollisionSound();
	fixed getHitRemovePower() { return removePower_; }
	fixed getHitPenetration() { return penetration_; }
	fixed getPower() { return power_; }
	Vector &getColor() { return color_; }
	ShieldLaserProofType getLaserProof() { return laserProof_; }
	ShieldMovementType getMovementProof() { return movementProof_; }

	virtual fixed getBoundingSize() = 0;
	virtual bool inShield(FixedVector &offset) = 0;
	virtual bool tankInShield(FixedVector &offset) = 0;
	virtual ShieldType getShieldType() = 0;
	virtual bool getRound() = 0;

protected:
	std::string collisionSound_;
	Vector color_;
	fixed removePower_;
	fixed penetration_;
	fixed power_;
	ShieldLaserProofType laserProof_;
	ShieldMovementType movementProof_;
};

#endif // !defined(AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
