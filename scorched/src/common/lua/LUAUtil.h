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

#if !defined(__INCLUDE_LUAUtil_INCLUDE__)
#define __INCLUDE_LUAUtil_INCLUDE__

#include <string>
#include <common/FixedVector.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

class LUAUtil
{
public:
	static void addVectorToStack(lua_State *L, const FixedVector &vector);
	static FixedVector getVectorFromStack(lua_State *L, int position);

	static bool tableContains(lua_State *L, int tablePosition, 
		const char *name);

	static fixed getNumberFromTable(lua_State *L, int tablePosition, 
		const char *name, fixed defaultResult);
	static int getIntFromTable(lua_State *L, int tablePosition, 
		const char *name, int defaultResult);
	static bool getBoolFromTable(lua_State *L, int tablePosition, 
		const char *name, bool defaultResult);
	static std::string getStringFromTable(lua_State *L, int tablePosition, 
		const char *name, const std::string &defaultResult);
};

#endif // __INCLUDE_LUAUtil_INCLUDE__
