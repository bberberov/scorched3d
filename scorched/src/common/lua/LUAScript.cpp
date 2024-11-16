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

#include <lua/LUAScript.h>
#include <lua/LUAUtil.h>
#include <lua/LUAS3DLib.h>
#include <lua/LUAS3DWeaponLib.h>
#include <common/Logger.h>

#include "lauxlib.h"
#include "lualib.h"

LUAScript::LUAScript(ScorchedContext *context) :
	context_(context),
	L_(0),
	weapon_(0)
{
	L_ = lua_open();
	
	// Load the available libraries
	luaopen_base(L_); 
	luaopen_table(L_); 
	luaopen_math(L_); 
	luaopen_string(L_); 
	luaopen_s3d(L_);

	// Store the context globaly
	lua_pushlightuserdata(L_, (void *) this);
	lua_setglobal(L_, "s3d_script");
}

LUAScript::~LUAScript()
{
	if (L_) 
	{
		lua_close(L_);
		L_ = 0;
	}
}

void LUAScript::addWeaponFunctions()
{
	luaopen_s3dweapon(L_);
}

bool LUAScript::loadFromFile(const std::string &filename, std::string &error)
{
	// Load the script
	bool result = true;
	int temp_int = luaL_dofile(L_, filename.c_str());
	if (temp_int != 0)
	{
		error = S3D::formatStringBuffer(
			"ERROR: LUA error : %s", lua_tostring(L_, -1));
		result = false;
	}
	
	return result;
}

bool LUAScript::setGlobal(const std::string &name, fixed value)
{
	lua_pushnumber(L_, (int) value.getInternalData());
	lua_setglobal(L_, name.c_str());
	return true;
}

bool LUAScript::functionExists(const std::string &functionName)
{
	lua_getglobal(L_, functionName.c_str());
	bool exists = !lua_isnil(L_, -1);
	lua_pop(L_, 1);
	return exists;
}

bool LUAScript::startFunction(const std::string &functionName)
{
	lua_getglobal(L_, functionName.c_str());
	return !lua_isnil(L_, -1);
}

bool LUAScript::endFunction(int argCount)
{
	int temp_int = lua_pcall(L_, argCount, 0, 0);
	if (temp_int != 0)
	{
		Logger::log(S3D::formatStringBuffer(
			"ERROR: LUA error : %s", lua_tostring(L_, -1)));
		return false;
	}
	return true;
}

bool LUAScript::addStringParameter(const std::string &str)
{
	lua_pushstring(L_, str.c_str());
	return true;
}

bool LUAScript::addNumberParameter(fixed number)
{
	lua_pushnumber(L_, (int) number.getInternalData());
	return true;
}

bool LUAScript::addBoolParameter(bool boolean)
{
	lua_pushboolean(L_, boolean);
	return true;
}

bool LUAScript::addVectorParameter(const FixedVector &v)
{
	LUAUtil::addVectorToStack(L_, v);
	return true;
}
