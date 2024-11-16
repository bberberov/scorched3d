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

#include <lua/LUAUtil.h>
#include <common/Logger.h>

void LUAUtil::addVectorToStack(lua_State *L, const FixedVector &vector)
{
	FixedVector &vec = (FixedVector &) vector;

	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, (int) vec[0].getInternalData());
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, (int) vec[1].getInternalData());
	lua_settable(L, -3);

	lua_pushstring(L, "z");
	lua_pushnumber(L, (int) vec[2].getInternalData());
	lua_settable(L, -3);
}

FixedVector LUAUtil::getVectorFromStack(lua_State *L, int position)
{
	FixedVector result;

	luaL_checktype(L, position, LUA_TTABLE);

	lua_pushstring(L, "x");
	lua_gettable(L, position);
	result[0] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	lua_pushstring(L, "y");
	lua_gettable(L, position);
	result[1] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	lua_pushstring(L, "z");
	lua_gettable(L, position);
	result[2] = fixed(true, luaL_checknumber(L, -1));
	lua_pop(L, 1);

	return result;
}

bool tableContains(lua_State *L, int tablePosition, const char *name)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);
	
	bool result = !lua_isnil(L, -1);
	lua_pop(L, 1);
	return result;
}

fixed LUAUtil::getNumberFromTable(lua_State *L, int tablePosition, const char *name, fixed defaultResult)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);
	
	fixed result = defaultResult;
	if (!lua_isnil(L, -1)) 
	{
		result = fixed(true, luaL_checknumber(L, -1));
	}
	lua_pop(L, 1);

	return result;
}

int LUAUtil::getIntFromTable(lua_State *L, int tablePosition, const char *name, int defaultResult)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);
	
	int result = defaultResult;
	if (!lua_isnil(L, -1)) 
	{
		fixed fresult = fixed(true, luaL_checknumber(L, -1));
		result = fresult.asInt();
	}
	lua_pop(L, 1);

	return result;
}

bool LUAUtil::getBoolFromTable(lua_State *L, int tablePosition, 
	const char *name, bool defaultResult)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);

	bool result = defaultResult;
	if (!lua_isnil(L, -1) &&
		lua_isboolean(L, -1))
	{
		result = (lua_toboolean(L, -1) != 0);
	}
	lua_pop(L, 1);

	return result;
}

std::string LUAUtil::getStringFromTable(lua_State *L, int tablePosition, 
	const char *name, const std::string &defaultResult)
{
	lua_pushstring(L, name);
	lua_gettable(L, tablePosition);

	std::string result = defaultResult;
	if (!lua_isnil(L, -1) &&
		lua_isstring(L, -1))
	{
		result = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	return result;
}
