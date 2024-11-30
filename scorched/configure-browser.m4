AC_ARG_WITH(
	[browser],
	[AS_HELP_STRING(
		[--with-browser],
		[use this external browser to open URLs on Linux (default: xdg-open)]
	)],
	[AS_IF(
		[test "x$with_browser" = 'xyes'],
		[with_browser=xdg-open],
		[with_browser=$withval]
	)],
	[with_browser=xdg-open]
)


AS_IF(
	[test "x$with_browser" != 'xno'],
	[
		AC_MSG_CHECKING([selected external browser])
		AC_MSG_RESULT([$with_browser])
		AC_CHECK_PROG(
			[USE_BROWSER],
			[$with_browser],
			[$with_browser])
		AS_IF(
			[test "x$USE_BROWSER" = 'x'],
			[AC_MSG_ERROR([
	Unable to find browser $with_browser.
	Use "--with-browser=" if not using 'xdg-open'])]
		)
	],
	[AC_MSG_WARN([
	You have disabled using an external browser.
	Clicking on links in Scorched3D will do nothing.])
	]
)
