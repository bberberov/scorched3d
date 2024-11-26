dnl Checks for static linking for wx windows
AC_ARG_WITH([wx-static],
            AC_HELP_STRING([--with-wx-static],
                           [enable static linking for wxwindows (default no)]),
            [use_static_wx=${withval}],,)

dnl Check for wxwindows
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.4.0, wxWin=1)
if test "$wxWin" != 1
then
	AC_MSG_ERROR([
                wxWindows must be installed on your system
                but the wx-config script couldn't be found.
                Please check that wx-config is in path, the directory
                where wxWindows libraries are installed (returned by
                'wx-config --libs' command) is in LD_LIBRARY_PATH or
                equivalent variable and wxWindows version is 2.4.0 or above
                Try http://www.wxwindows.org to download wxwindows.
	])
fi

if test "x$use_static_wx" = 'xyes'
then
	WX_LIBS="$WX_LIBS_STATIC"
fi
