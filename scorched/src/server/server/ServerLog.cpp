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

#include <server/ServerLog.h>
#include <common/Logger.h>

ServerLog *ServerLog::instance_ = 0;

ServerLog *ServerLog::instance()
{
	if (!instance_) instance_ = new ServerLog;
	return instance_;
}

ServerLog::ServerLog()
{
	Logger::addLogger(this);
}

ServerLog::~ServerLog()
{
}

void ServerLog::logMessage(LoggerInfo &info)
{
	ServerLogEntry entry;
	entry.text += info.getTime();
	entry.text += " - ";
	entry.text += info.getMessage();
	entries_.push_back(entry);

	if (entries_.size() > 500)
	{
		entries_.pop_front();
	}
}
