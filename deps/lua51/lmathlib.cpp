/*
** $Id: lmathlib.c,v 1.67 2005/08/26 17:36:32 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


static int math_abs (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.abs();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_sin (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.sin();
	lua_pushnumber(L,(int)  result.getInternalData());
	return 1;
}

static int math_cos (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.cos();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_tan (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.tan();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_acos (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.acos();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_ceil (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.ceil();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_floor (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.floor();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_sqrt (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.sqrt();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_pow (lua_State *L)
{
	fixed param1(true, luaL_checknumber(L, 1));
	fixed param2(true, luaL_checknumber(L, 2));
	fixed result = param1.pow(param2);
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_log (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.log();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_log10 (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.log10();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_exp (lua_State *L) 
{
	fixed param(true, luaL_checknumber(L, 1));
	fixed result = param.exp();
	lua_pushnumber(L, (int) result.getInternalData());
	return 1;
}

static int math_min (lua_State *L) 
{
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}

static int math_max (lua_State *L) 
{
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}


static const luaL_Reg mathlib[] = {
  {"abs",   math_abs},
  {"acos",  math_acos},
//  {"asin",  math_asin},
//  {"atan2", math_atan2},
//  {"atan",  math_atan},
  {"ceil",  math_ceil},
//  {"cosh",   math_cosh},
  {"cos",   math_cos},
//  {"deg",   math_deg},
  {"exp",   math_exp},
  {"floor", math_floor},
//  {"fmod",   math_fmod},
//  {"frexp", math_frexp},
//  {"ldexp", math_ldexp},
  {"log10", math_log10},
  {"log",   math_log},
  {"max",   math_max},
  {"min",   math_min},
//  {"modf",   math_modf},
  {"pow",   math_pow},
//  {"rad",   math_rad},
//  {"sinh",   math_sinh},
  {"sin",   math_sin},
  {"sqrt",  math_sqrt},
//  {"tanh",   math_tanh},
  {"tan",   math_tan},
  {NULL, NULL}
};


/*
** Open math library
*/
LUALIB_API int luaopen_math (lua_State *L) {
  luaL_register(L, LUA_MATHLIBNAME, mathlib);
  return 1;
}

