dnl Checks for mysql
MYSQL_LIBS=
AC_ARG_WITH([mysql],
            AC_HELP_STRING([--with-mysql],
                           [enable mysql support (default no)]),
            [use_mysql=${withval}],,)
AH_TEMPLATE(HAVE_MYSQL,[Define if you have libmysql])

if test "x$use_mysql" = 'xyes'
then
	AC_MSG_CHECKING(for MySql support)
	have_mysql=no
	AC_TRY_COMPILE([
		#include <mysql/mysql.h>
		],[
		],[
		have_mysql=yes
	])
	AC_MSG_RESULT($have_mysql)

	if test "x$have_mysql" != 'xyes'
	then
		AC_MSG_ERROR([*** Can't find the MySql library Try: http://www.mysql.com])
	else
		MYSQL_CFLAGS=`mysql_config --cflags`
		MYSQL_CFLAGS="$MYSQL_CFLAGS -DHAVE_MYSQL=1"
		MYSQL_LIBS=`mysql_config --libs`
	fi
fi
AC_SUBST(MYSQL_LIBS)
AC_SUBST(MYSQL_CFLAGS)

dnl Checks for pgsql
PGSQL_LIBS=
AC_ARG_WITH([pgsql],
            AC_HELP_STRING([--with-pgsql],
                           [enable pgsql support (default no)]),
            [use_pgsql=${withval}],,)
AH_TEMPLATE(HAVE_PGSQL,[Define if you have libpq])

if test "x$use_pgsql" = 'xyes'
then
	AC_MSG_CHECKING(for PGSql support)
	have_pgsql=no
	AC_TRY_COMPILE([
		#include <pgsql/libpq-fe.h>
		],[
		],[
		have_pgsql=yes
	])
	AC_MSG_RESULT($have_pgsql)

	if test "x$have_pgsql" != 'xyes'
	then
		AC_MSG_ERROR([*** Can't find the PGSql library Try: http://www.postgresql.org])
	else
		AC_DEFINE(HAVE_PGSQL)
		PGSQL_LIBS="-lpq"
	fi
fi
AC_SUBST(PGSQL_LIBS)
