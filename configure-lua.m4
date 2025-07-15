dnl Checks for Lua 5.1.x

AC_ARG_WITH(
	[system-lua],
	[AS_HELP_STRING(
		[--with-system-lua],
		[use system Lua, with pkg-config string (default: lua-5.1)]
	)],
	[AS_IF(
		[test "x$with_system_lua" = 'xyes'],
		[with_system_lua=lua-5.1],
		[with_system_lua=$withval]
	)],
	[with_system_lua=lua-5.1]
)

AS_IF(
	[test "x$with_system_lua" != 'xno'],
	[PKG_CHECK_MODULES(
		[LUA],
		[$with_system_lua],
		[],
		[AC_MSG_ERROR([
	Lua 5.1.x not found, using pkg-config with name: "$with_system_lua"
	See: https://www.lua.org/])]
	)],
	[
		AC_MSG_ERROR([
	Using the bundeled Lua 5.1.2 is currently not possible])
		# LUA_CFLAGS=
		# LUA_LIBS=
		# AC_SUBST(LUA_CFLAGS)
		# AC_SUBST(LUA_LIBS)
	]
)

