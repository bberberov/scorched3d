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

#if !defined(__INCLUDE_EventHandlerDataBaseMySQLh_INCLUDE__)
#define __INCLUDE_EventHandlerDataBaseMySQLh_INCLUDE__

#if defined(_WIN32)
#include <Winsock2.h>
#endif
#include <events/EventHandlerDataBase.h>
#include <mysql/mysql.h>

class Weapon;
class EventHandlerDataBaseMySQL : public EventHandlerDataBase
{
public:
	EventHandlerDataBaseMySQL();
	virtual ~EventHandlerDataBaseMySQL();

protected:
	MYSQL *mysql_;

	virtual bool runQuery(const char *, ...);
	virtual std::list<EventHandlerDataBase::RowResult> runSelectQuery(const char *, ...);
	virtual bool connectDatabase(const char *host, const char *port,
		const char *user, const char *passwd, 
		const char *db);

	virtual int getLastInsertId();
	virtual void escapeString(char *to, const char *from, unsigned long length);

};

#endif 

#endif // HAVE_MYSQL
