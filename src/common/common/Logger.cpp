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

#include <common/Defines.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>

// ************************************************
// NOTE: This logger is and needs to be thread safe
// ************************************************

static SDL_mutex *logMutex_ = 0;
static Uint32 threadId = (Uint32) -1;
Logger * Logger::instance_ = 0;

Logger * Logger::instance()
{
	if (!instance_)
	{
		instance_ = new Logger;
	}
	return instance_;
}

Logger::Logger()
{
	if (!logMutex_) logMutex_ = SDL_CreateMutex();
}

Logger::~Logger()
{
}

void Logger::addLogger(LoggerI *logger)
{
	Logger::instance();
	SDL_LockMutex(logMutex_);
	Logger::instance()->loggers_.push_back(logger);
	SDL_UnlockMutex(logMutex_);
}

void Logger::remLogger(LoggerI *logger)
{
	Logger::instance();
	std::list<LoggerI *>::iterator itor;
	SDL_LockMutex(logMutex_);
	itor = Logger::instance()->loggers_.begin();
	while(itor != Logger::instance()->loggers_.end()) {
		if (*itor == logger) {
			Logger::instance()->loggers_.erase(itor);
			break;
		}
		++itor;
	}
	SDL_UnlockMutex(logMutex_);
}

void Logger::log(const std::string &text)
{
	log(text.c_str());
}

void Logger::log(const char *text)
{
	if (!text) return;

	Logger::instance();

	SDL_LockMutex(logMutex_);
	
	LoggerInfo info;
	info.setMessage(text);
	addLog(info);

	SDL_UnlockMutex(logMutex_);

	if (SDL_ThreadID() == threadId)
	{
		processLogEntries();
	}
}

void Logger::log(const LoggerInfo &info)
{
	Logger::instance();

	SDL_LockMutex(logMutex_);
	addLog((LoggerInfo &) info);
	SDL_UnlockMutex(logMutex_);
}

void Logger::addLog(LoggerInfo &info)
{
	// Add the time to the beginning of the log message
	info.setTime();

	// Add single or multiple lines
	char *found = (char *) strchr(info.getMessage(), '\n');
	char *start = (char *) info.getMessage();
	if (found)
	{
		while (found)
		{
			*found = '\0';
			LoggerInfo *newInfo = new LoggerInfo(info);
			newInfo->setMessage(start);
			instance_->entries_.push_back(newInfo);
			start = found;
			start++;

			found = strchr(start, '\n');

			/*if (SDL_ThreadID() == threadId)
			{
				processLogEntries();
			}*/
		}
		if (start[0] != '\0')
		{
			LoggerInfo *newInfo = new LoggerInfo(info);
			newInfo->setMessage(start);
			instance_->entries_.push_back(newInfo);
		}
	}
	else
	{
		instance_->entries_.push_back(
			new LoggerInfo(info));
	}
}

void Logger::processLogEntries()
{
	if (threadId == (Uint32) -1)
	{
		threadId = SDL_ThreadID();
	}

	Logger::instance();

	SDL_LockMutex(logMutex_);
	std::list<LoggerInfo *> &entries = Logger::instance()->entries_;
	while (!entries.empty())
	{
		LoggerInfo *firstEntry = entries.front();

		std::list<LoggerI *> &loggers = Logger::instance()->loggers_;
		std::list<LoggerI *>::iterator logItor;
		for (logItor = loggers.begin();
			logItor != loggers.end();
			++logItor)
		{
			LoggerI *log = (*logItor);
			log->logMessage(*firstEntry);
		}

		entries.pop_front();
		delete firstEntry;
	}
	SDL_UnlockMutex(logMutex_);
}

