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

#include <server/ServerConsoleProgressCounter.h>
#include <common/Logger.h>

ServerConsoleProgressCounter *ServerConsoleProgressCounter::instance_ = 0;

ServerConsoleProgressCounter *ServerConsoleProgressCounter::instance()
{
	if (!instance_) instance_ = new ServerConsoleProgressCounter();
	return instance_;
}

ServerConsoleProgressCounter::ServerConsoleProgressCounter() :
	hashes_(25)
{
	progressCounter_.setUser(this);
	Logger::instance()->addLogger(this);
}

ServerConsoleProgressCounter::~ServerConsoleProgressCounter()
{
}

void ServerConsoleProgressCounter::drawHashes(int neededHashes)
{
	if (hashes_ < neededHashes)
	{
		for (int h=hashes_;h<neededHashes; h++)
		{
			printf("#");
			if (h == 24)
			{
				printf("\n");
			}
		}
		hashes_ = neededHashes;
	}
	fflush(stdout);
}

void ServerConsoleProgressCounter::logMessage(LoggerInfo &info)
{
	drawHashes(25);
}

void ServerConsoleProgressCounter::operationChange(const LangString &op)
{
	Logger::processLogEntries();
	hashes_ = 0;
}

void ServerConsoleProgressCounter::progressChange(const LangString &op, const float percentage)
{
	int neededHashes = int(percentage / 4.0f);
	drawHashes(neededHashes);
}
