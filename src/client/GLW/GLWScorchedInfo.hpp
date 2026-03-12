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

#ifndef __INCLUDE_GLWScorchedInfo_hpp_INCLUDE__
#define __INCLUDE_GLWScorchedInfo_hpp_INCLUDE__

#include <GLW/GLWidget.hpp>
#include <XML/XMLParser.hpp>
#include <common/Vector.hpp>

class GLWHudCondition : public GLWCondition
{
public:
	GLWHudCondition();
	virtual ~GLWHudCondition();

	virtual bool getResult( GLWidget* widget );

	REGISTER_CLASS_HEADER( GLWHudCondition );
};

class GLWScorchedInfo : public GLWidget
{
public:
	enum InfoType
	{
		eNone,
		eWind,
		ePlayerName,
		ePlayerIcon,
		ePlayerRank,
		eAutoDefenseCount,
		eParachuteCount,
		eHealthCount,
		eShieldCount,
		eFuelCount,
		eBatteryCount,
		eWeaponName,
		eWeaponCount,
		eWeaponIcon,
		eRotation,
		eRotationDiff,
		eElevation,
		eElevationDiff,
		ePower,
		ePowerDiff
	};

	GLWScorchedInfo( float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f );
	virtual ~GLWScorchedInfo();

	virtual void draw();
	virtual void mouseDown( int button, float x, float y, bool& skipRest );
	virtual bool initFromXML( XMLNode* node );

	REGISTER_CLASS_HEADER( GLWScorchedInfo );

protected:
	InfoType infoType_;
	float    fontSize_;
	Vector   fontColor_;
	Vector   selectedColor_;
	bool     noCenter_;
};

#endif  // __INCLUDE_GLWScorchedInfo_hpp_INCLUDE__
