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

#include <weapons/ShieldRound.h>
#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(ShieldRound);

ShieldRound::ShieldRound() : 
	glow_(true)
{
}

ShieldRound::~ShieldRound()
{
}

Shield::ShieldType ShieldRound::getShieldType()
{
	return ShieldTypeRoundNormal;
}

bool ShieldRound::inShield(FixedVector &offset)
{
	if (offset.Magnitude() <= radius_)
	{
		if (!halfShield_) return true;

		FixedVector normal = offset.Normalize();
		FixedVector up(0, 0, 1);
		if (normal.dotP(up) > fixed(true, 7000))
		{
			return true;
		}
	}
	return false;
}

bool ShieldRound::tankInShield(FixedVector &offset)
{
	if (offset.Magnitude() <= radius_)
	{
		return true;
	}
	return false;
}

bool ShieldRound::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Shield::parseXML(context, accessoryNode)) return false;

	// Get the penetration
	if (!accessoryNode->getNamedChild("radius", radius_)) return false;
	if (radius_ <= 0) return accessoryNode->returnError("ShieldRound radius must be > 0");

	// Get the half size
	if (!accessoryNode->getNamedChild("halfshield", halfShield_)) return false;

	accessoryNode->getNamedChild("glow", glow_, false);

	return true;
}
