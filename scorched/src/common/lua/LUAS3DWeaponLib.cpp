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

#include <lua/LUAS3DWeaponLib.h>
#include <lua/LUAScript.h>
#include <lua/LUAUtil.h>
#include <weapons/AccessoryStore.h>
#include <actions/Explosion.h>
#include <actions/ExplosionParams.h>
#include <actions/Napalm.h>
#include <actions/Laser.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <common/Logger.h>

#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static LUAScript *getScript(lua_State *L)
{
	lua_getglobal(L, "s3d_script");
	DIALOG_ASSERT(!lua_isnil(L, -1));
	LUAScript *script = (LUAScript *) lua_touserdata(L, -1);
	lua_pop(L, 1);
	return script;
}

static int s3d_random(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);
	fixed result = wrapper->getContext()->
		getSimulator().getRandomGenerator().getRandFixed("LUA");

	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int s3d_fire_weapon(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);

	const char *weaponName = luaL_checkstring(L, 1);
	int playerId = luaL_checknumber(L, 2);
	FixedVector position = LUAUtil::getVectorFromStack(L, 3);
	FixedVector velocity = LUAUtil::getVectorFromStack(L, 4);

	Accessory *accessory =
		wrapper->getContext()->getAccessoryStore().findByPrimaryAccessoryName(weaponName);
	if (!accessory) 
	{
		Logger::log(S3D::formatStringBuffer("Failed to find accessory named %s", weaponName));
		return 0;
	}

	AccessoryPart *accessoryPart = accessory->getAction();
	if (!accessoryPart || accessoryPart->getType() != AccessoryPart::AccessoryWeapon)
	{
		Logger::log(S3D::formatStringBuffer("Accessory named %s is not a weapons", weaponName));
		return 0;
	}

	Weapon *weapon = (Weapon*) accessoryPart;

	WeaponFireContext weaponContext(playerId, 0, 0, FixedVector(), true, true);
	weapon->fire(*wrapper->getContext(), weaponContext, position, velocity);

	return 0;
}

static int s3d_explosion(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);
	ExplosionParams *explosionParams = new ExplosionParams();

	unsigned int playerId = (unsigned int) luaL_checknumber(L, 1);
	FixedVector position = LUAUtil::getVectorFromStack(L, 2);
	FixedVector velocity = LUAUtil::getVectorFromStack(L, 3);
	explosionParams->parseLUA(L, 4);	

	WeaponFireContext fireContext(playerId, 0, 0, FixedVector(), true, true);
	Explosion *explosion = new Explosion(
		position, velocity, explosionParams, wrapper->getWeapon(), fireContext);
	wrapper->getContext()->getActionController().addAction(explosion);

	return 0;
}

static int s3d_napalm(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);
	NapalmParams *napalmParams = new NapalmParams();

	unsigned int playerId = (unsigned int) luaL_checknumber(L, 1);
	FixedVector position = LUAUtil::getVectorFromStack(L, 2);
	napalmParams->parseLUA(L, 3);	

	WeaponFireContext fireContext(playerId, 0, 0, FixedVector(), true, true);
	Napalm *napalm = new Napalm(
		position[0].asInt(), position[1].asInt(),
		wrapper->getWeapon(), napalmParams, fireContext);
	wrapper->getContext()->getActionController().addAction(napalm);

	return 0;
}

static int s3d_laser(lua_State *L) 
{
	LUAScript *wrapper = getScript(L);
	LaserParams *laserParams = new LaserParams();

	unsigned int playerId = (unsigned int) luaL_checknumber(L, 1);
	FixedVector position = LUAUtil::getVectorFromStack(L, 2);
	FixedVector direction = LUAUtil::getVectorFromStack(L, 3);
	laserParams->parseLUA(L, 4);

	WeaponFireContext fireContext(playerId, 0, 0, FixedVector(), true, true);
	Laser *laser = new Laser(
		wrapper->getWeapon(), laserParams,
		position, direction,
		fireContext);
	wrapper->getContext()->getActionController().addAction(laser);

	return 0;
}

static const luaL_Reg s3dweaponlib[] = {
	{"fire_weapon", s3d_fire_weapon}, 
	{"explosion", s3d_explosion},
	{"napalm", s3d_napalm},
	{"laser", s3d_laser},
	{"random", s3d_random},
	{NULL, NULL}
};

LUALIB_API int luaopen_s3dweapon (lua_State *L) {
	luaL_register(L, LUA_S3DWEAPONLIBNAME, s3dweaponlib);
	return 1;
}
