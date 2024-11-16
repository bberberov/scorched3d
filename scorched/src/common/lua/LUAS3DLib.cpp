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

#include <lua/LUAS3DLib.h>
#include <lua/LUAScript.h>
#include <lua/LUAUtil.h>
#include <common/OptionEntry.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <weapons/AccessoryStore.h>
#include <tanket/TanketAccessories.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <target/TargetLife.h>
#include <landscapemap/LandscapeMaps.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static LUAScript *getScript(lua_State *L)
{
	lua_getglobal(L, "s3d_script");
	LUAScript *script = (LUAScript *) lua_touserdata(L, -1);
	lua_pop(L, 1);
	return script;
}

static void addTank(lua_State *L, Tank *tank)
{
	lua_newtable(L);

	lua_pushstring(L, "name");
	lua_pushstring(L, tank->getCStrName().c_str());
	lua_settable(L, -3);

	lua_pushstring(L, "id");
	lua_pushnumber(L, tank->getPlayerId());
	lua_settable(L, -3);

	lua_pushstring(L, "position");
	LUAUtil::addVectorToStack(L, tank->getLife().getCenterPosition());
	lua_settable(L, -3);

	lua_pushstring(L, "alive");
	lua_pushboolean(L, tank->getAlive()?1:0);
	lua_settable(L, -3);

	lua_pushstring(L, "team");
	lua_pushnumber(L, lua_Number(tank->getTeam() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);

	lua_pushstring(L, "money");
	lua_pushnumber(L, lua_Number(tank->getScore().getMoney() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);

	lua_pushstring(L, "score");
	lua_pushnumber(L, lua_Number(tank->getScore().getScore() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);

	lua_pushstring(L, "rank");
	lua_pushnumber(L, lua_Number(tank->getScore().getRank() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);

	lua_pushstring(L, "skill");
	lua_pushnumber(L, lua_Number(tank->getScore().getSkill() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);

	lua_pushstring(L, "lives");
	lua_pushnumber(L, lua_Number(tank->getState().getLives() * fixed::FIXED_RESOLUTION));
	lua_settable(L, -3);
}

static int s3d_get_option(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	const char *optionName = luaL_checkstring(L, 1);
	OptionEntry *entry = OptionEntryHelper::getEntry(
		wrapper->getContext()->getOptionsGame().getMainOptions().getOptions(),
		optionName);
	if (!entry) 
	{
		entry = OptionEntryHelper::getEntry(
			wrapper->getContext()->getOptionsTransient().getOptions(),
			optionName);
	}
	if (entry) 
	{
		if (entry->getEntryType() == OptionEntry::OptionEntryBoolType)
		{
			lua_pushboolean(L, ((OptionEntryBool *)entry)->getValue());
		}
		else if (entry->getEntryType() == OptionEntry::OptionEntryIntType ||
			entry->getEntryType() == OptionEntry::OptionEntryBoundedIntType ||
			entry->getEntryType() == OptionEntry::OptionEntryEnumType)
		{
			lua_pushnumber(L, lua_Number(((OptionEntryInt *)entry)->getValue() * fixed::FIXED_RESOLUTION));
		}
		else if (entry->getEntryType() == OptionEntry::OptionEntryFloatType)
		{
			lua_pushnumber(L, (unsigned int)(((OptionEntryFloat *)entry)->getValue() * fixed::FIXED_RESOLUTION));
		}
		else if (entry->getEntryType() == OptionEntry::OptionEntryFixedType)
		{
			lua_pushnumber(L, (int) ((OptionEntryFixed *)entry)->getValue().getInternalData());
		}
		else if (entry->getEntryType() == OptionEntry::OptionEntryFixedVectorType)
		{
			LUAUtil::addVectorToStack(L, ((OptionEntryFixedVector *)entry)->getValue());
		}
		else
		{
			lua_pushstring(L, entry->getValueAsString());
		}
	}
	else 
	{
		Logger::log(S3D::formatStringBuffer("s3d_get_option:Failed to an option named %s", optionName));
		lua_pushstring(L, "");
	}

	return 1;
}

static int s3d_get_tank(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	int number = luaL_checknumber(L, 1);
	Tank *tank =
		wrapper->getContext()->getTargetContainer().getTankById((unsigned int) number);
	if (tank) addTank(L, tank);
	else
	{
		Logger::log(S3D::formatStringBuffer("s3d_get_tank:Failed to an tank id %u", 
			(unsigned int) number));
		lua_pushstring(L, "");
	}

	return 1;
}

static int s3d_get_tanks(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	std::map<unsigned int, Tank *> &tanks =
		wrapper->getContext()->getTargetContainer().getTanks();
	lua_newtable(L);

	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin(); 
		itor != tanks.end();
		++itor)
	{
		Tank *tank = itor->second;
		lua_pushnumber(L, tank->getPlayerId());
		addTank(L, tank);
		lua_settable(L, -3);
	}

	return 1;
}

static int s3d_get_height(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	fixed x = fixed(true, luaL_checknumber(L, 1));
	fixed y = fixed(true, luaL_checknumber(L, 2));

	fixed result = wrapper->getContext()->getLandscapeMaps().getGroundMaps().getHeight(
		x.asInt(), y.asInt());
	lua_pushnumber(L, (int) result.getInternalData());

	return 1;
}

static int s3d_get_arenawidth(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	fixed result(wrapper->getContext()->getLandscapeMaps().getGroundMaps().getArenaWidth());
	lua_pushnumber(L, (int) result.getInternalData());

	return 1;
}

static int s3d_get_arenaheight(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	fixed result(wrapper->getContext()->getLandscapeMaps().getGroundMaps().getArenaHeight());
	lua_pushnumber(L, (int) result.getInternalData());

	return 1;
}

static int s3d_get_landscapewidth(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	fixed result(wrapper->getContext()->getLandscapeMaps().getGroundMaps().getLandscapeWidth());
	lua_pushnumber(L, (int) result.getInternalData());

	return 1;
}

static int s3d_get_landscapeheight(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	fixed result(wrapper->getContext()->getLandscapeMaps().getGroundMaps().getLandscapeHeight());
	lua_pushnumber(L, (int) result.getInternalData());

	return 1;
}

static int s3d_set_tank_score(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	int number = luaL_checknumber(L, 1);
	int score = int(luaL_checknumber(L, 2) / fixed::FIXED_RESOLUTION);

	Tank *tank =
		wrapper->getContext()->getTargetContainer().getTankById((unsigned int) number);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("s3d_set_tank_score:Failed to an tank id %u", 
			(unsigned int) number));
		return 0;
	}
	tank->getScore().setScore(score);

	return 0;
}

static int s3d_set_tank_money(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	int number = luaL_checknumber(L, 1);
	int money = int(luaL_checknumber(L, 2) / fixed::FIXED_RESOLUTION);

	Tank *tank =
		wrapper->getContext()->getTargetContainer().getTankById((unsigned int) number);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("s3d_set_tank_money:Failed to an tank id %u", 
			(unsigned int) number));
		return 0;
	}
	tank->getScore().setMoney(money);

	return 0;
}

static int s3d_set_tank_position(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	int number = luaL_checknumber(L, 1);
	FixedVector position = LUAUtil::getVectorFromStack(L, 2);

	Tank *tank =
		wrapper->getContext()->getTargetContainer().getTankById((unsigned int) number);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("s3d_set_tank_money:Failed to an tank id %u", 
			(unsigned int) number));
		return 0;
	}
	tank->getLife().setTargetPosition(position);

	return 0;
}

static int s3d_give_weapon(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	int number = luaL_checknumber(L, 1);
	const char *weaponName = luaL_checkstring(L, 2);
	int count = int(luaL_checknumber(L, 3) / fixed::FIXED_RESOLUTION);

	Tank *tank =
		wrapper->getContext()->getTargetContainer().getTankById((unsigned int) number);
	if (!tank)
	{
		Logger::log(S3D::formatStringBuffer("s3d_give_weapon:Failed to an tank id %u", 
			(unsigned int) number));
		return 0;
	}

	Accessory *accessory =
		wrapper->getContext()->getAccessoryStore().findByPrimaryAccessoryName(weaponName);
	if (!accessory) 
	{
		Logger::log(S3D::formatStringBuffer("s3d_give_weaponFailed to find accessory named %s", weaponName));
		return 0;
	}

	tank->getAccessories().add(accessory, count, false);

	return 0;
}

static const luaL_Reg s3dlib[] = {
	{"get_option", s3d_get_option},
	{"get_tank", s3d_get_tank},
	{"get_tanks", s3d_get_tanks},
	{"set_tank_score", s3d_set_tank_score},
	{"set_tank_money", s3d_set_tank_money},
	{"set_tank_position", s3d_set_tank_position},
	{"give_weapon", s3d_give_weapon},
	{"get_height", s3d_get_height},
	{"get_arenawidth", s3d_get_arenawidth},
	{"get_arenaheight", s3d_get_arenaheight},
	{"get_landscapewidth", s3d_get_landscapewidth},
	{"get_landscapeheight", s3d_get_landscapeheight},
	{NULL, NULL}
};

LUALIB_API int luaopen_s3d (lua_State *L) {
	luaL_register(L, LUA_S3DLIBNAME, s3dlib);
	return 1;
}
