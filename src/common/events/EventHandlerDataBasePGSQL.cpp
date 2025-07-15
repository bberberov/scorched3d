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

#ifdef HAVE_PGSQL
/* Code contains several string format bugs, and at least one buffer overflow */

#include <events/EventHandlerDataBasePGSQL.h>
#include <common/Logger.h>

EventHandlerDataBasePGSQL::EventHandlerDataBasePGSQL() : pgsql_(NULL), lastresult_ (NULL)
{

}

EventHandlerDataBasePGSQL::~EventHandlerDataBasePGSQL()
{
}

#define SQL_BUFFER_SIZE 8192
virtual bool EventHandlerDataBasePGSQL::runQuery(const char *fmt, ...)
{
    if (!success_) return false;

    if(lastresult_) {
        PQclear(lastresult_);
        lastresult_ = NULL;
    }

    static char text[SQL_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    int sqlLen = vsnprintf(text, SQL_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if(sqlLen >= SQL_BUFFER_SIZE) {
        Logger::log("pgsql: Query failed, too long.\n");
        return false;
    }

    lastresult_ = PQexec(pgsql_, text);
    
    return lastresult_ && (
            PQresultStatus(lastresult_) == PGRES_COMMAND_OK ||
            PQresultStatus(lastresult_) == PGRES_TUPLES_OK
            );
}

bool EventHandlerDataBasePGSQL::connectDatabase(const char *host, const char *user, 
	const char *passwd, const char *db)
{
      pgsql_ = PQsetdbLogin(
                host,
                NULL,
                NULL,
                NULL,
                db,
                user,
                passwd);
    if (pgsql_ && PQstatus(pgsql_) == CONNECTION_OK)
    {
        Logger::log("pgsql stats logger started");
    }
    else
    {
		Logger::log(S3D::formatStringBuffer("pgsql stats logger failed to start. "
                "Error: %s",
                PQerrorMessage(pgsql_)));
		Logger::log(S3D::formatStringBuffer("pgsql params : host %s, user %s, passwd %s, db %s",
                host, user,
                passwd, db));
        if(pgsql_) {
            PQfinish(pgsql_);
            pgsql_ = NULL;
        }
		return false;
    }

	return true;
}

#endif // HAVE_PGSQL
