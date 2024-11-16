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

#ifdef HAVE_MYSQL

#include <events/EventHandlerDataBaseMySQL.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <stdlib.h>

EventHandlerDataBaseMySQL::EventHandlerDataBaseMySQL() : mysql_(0)
{

}

EventHandlerDataBaseMySQL::~EventHandlerDataBaseMySQL()
{
}

bool EventHandlerDataBaseMySQL::runQuery(const char *format, ...)
{
	va_list ap; 
	va_start(ap, format); 
	std::string text = S3D::formatStringList(format, ap);
	va_end(ap); 

	return (mysql_real_query(mysql_, text.c_str(), (int) text.size()) == 0);
}

std::list<EventHandlerDataBase::RowResult> EventHandlerDataBaseMySQL::runSelectQuery(const char *format, ...)
{
	std::list<EventHandlerDataBase::RowResult> results;

	va_list ap; 
	va_start(ap, format); 
	std::string text = S3D::formatStringList(format, ap);
	va_end(ap); 

	if (mysql_real_query(mysql_, text.c_str(), (int) text.size()) != 0) return results;

	MYSQL_RES *result = mysql_store_result(mysql_);
	if (result)
	{
		int rows = (int) mysql_num_rows(result);
		int cols = (int) mysql_num_fields(result);
		MYSQL_FIELD *fields = mysql_fetch_fields(result);
		for (int r=0; r<rows; r++)
		{
			EventHandlerDataBase::RowResult rowResult;
			MYSQL_ROW row = mysql_fetch_row(result);

			for (int c=0; c<cols; c++)
			{
				const char *value = (row[c]?row[c]:"");
				const char *name = fields[c].name;
				rowResult.columns.push_back(value);
				rowResult.names[name] = c;
			}

			results.push_back(rowResult);
		}
		mysql_free_result(result);
	}

	return results;
}

bool EventHandlerDataBaseMySQL::connectDatabase(const char *host, const char *port,
	const char *user, const char *passwd, 
	const char *db)
{
    mysql_ = mysql_init(0);
	if (!mysql_)
	{
		Logger::log( "Failed to init mysql");
		return false;
	}
	mysql_->reconnect = 1;

	int connectPort = 0;
	const char *connectSocket = 0;
	if (0 == stricmp(host, "localhost"))
	{
		connectSocket = port;
	}
	else
	{
		connectPort = atoi(port);
	}

	if (!mysql_real_connect(
		mysql_,
		host,
		user,
		passwd,
		db,
		connectPort, 
		connectSocket, 0))
	{
		Logger::log(S3D::formatStringBuffer("mysql stats logger failed to start. "
			"Error: %s",
			mysql_error(mysql_)));
		Logger::log(S3D::formatStringBuffer("mysql params : host %s, user %s, passwd %s, db %s",
			host, user, passwd, db));
		return false;
	}

	return true;
}

int EventHandlerDataBaseMySQL::getLastInsertId()
{
	return (int) mysql_insert_id(mysql_);
}

void EventHandlerDataBaseMySQL::escapeString(char *to, const char *from, unsigned long length)
{
	mysql_real_escape_string(mysql_, to, from, length);
}

#endif // HAVE_MYSQL
