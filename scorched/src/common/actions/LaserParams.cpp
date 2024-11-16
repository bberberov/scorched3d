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

#include <actions/LaserParams.h>
#include <lua/LUAUtil.h>
#include <XML/XMLNode.h>

LaserParams::LaserParams() : 
	hurtFirer_(false),
	color_(1.0f, 0.0f, 0.0f),
	totalTime_(1),
	minimumDistance_(10), maximumDistance_(200),
	minimumHurt_(34), maximumHurt_(40),
	hurtRadius_(2),
	ringTexture_("ring"),
	ringRadius_(0.0f)
{
}

LaserParams::~LaserParams()
{
}

bool LaserParams::parseXML(XMLNode *accessoryNode)
{
	if (!accessoryNode->getNamedChild("color", color_)) return false;
	accessoryNode->getNamedChild("hurtfirer", hurtFirer_, false);
	accessoryNode->getNamedChild("ringtextureset", ringTexture_, false);
	accessoryNode->getNamedChild("ringradius", ringRadius_, false);

	return true;
}

void LaserParams::parseLUA(lua_State *L, int position)
{
	luaL_checktype(L, position, LUA_TTABLE);

	minimumHurt_ = LUAUtil::getNumberFromTable(L, position, "minimumhurt", minimumHurt_);
	maximumHurt_ = LUAUtil::getNumberFromTable(L, position, "maximumhurt", maximumHurt_);
	minimumDistance_ = LUAUtil::getNumberFromTable(L, position, "minimumdistance", minimumDistance_);
	maximumDistance_ = LUAUtil::getNumberFromTable(L, position, "maximumdistance", maximumDistance_);
	hurtRadius_ = LUAUtil::getNumberFromTable(L, position, "hurtradius", hurtRadius_);
	totalTime_ = LUAUtil::getNumberFromTable(L, position, "totaltime", totalTime_);
	hurtFirer_ = LUAUtil::getBoolFromTable(L, position, "hurtfirer", hurtFirer_);
}
