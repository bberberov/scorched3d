////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_ShieldRound_hpp_INCLUDE__
#define __INCLUDE_ShieldRound_hpp_INCLUDE__

#include <weapons/Shield.hpp>

class ShieldRound : public Shield
{
public:
	ShieldRound();
	virtual ~ShieldRound();

	virtual bool parseXML( AccessoryCreateContext& context, XMLNode* accessoryNode );

	// ShieldRound attributes
	// clang-format off
	// uncrustify off
	fixed getActualRadius() { return radius_; }
	bool  getHalfShield()   { return halfShield_; }
	bool  getGlow()         { return glow_; }
	// uncrustify on
	// clang-format on

	virtual bool       inShield( FixedVector& offset );
	virtual bool       tankInShield( FixedVector& offset );
	virtual ShieldType getShieldType();

	// clang-format off
	// uncrustify off
	virtual bool  getRound()        { return true; }
	virtual fixed getBoundingSize() { return radius_; }
	// uncrustify on
	// clang-format on

	REGISTER_ACCESSORY_HEADER( ShieldRound, AccessoryPart::AccessoryShield );

protected:
	fixed radius_;
	bool  halfShield_;
	bool  glow_;
};

#endif  // __INCLUDE_ShieldRound_hpp_INCLUDE__
