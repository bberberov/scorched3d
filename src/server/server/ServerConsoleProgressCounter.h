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

#if !defined(__INCLUDE_ServerConsoleProgressCounterh_INCLUDE__)
#define __INCLUDE_ServerConsoleProgressCounterh_INCLUDE__

#include <common/LoggerI.h>
#include <common/ProgressCounter.h>

class ServerConsoleProgressCounter : 
	public ProgressCounterI, 
	public LoggerI
{
public:
	static ServerConsoleProgressCounter *instance();

	ProgressCounter *getProgressCounter() { return &progressCounter_; }

	virtual void drawHashes(int neededHashes);
	virtual void logMessage(LoggerInfo &info);
	virtual void operationChange(const LangString &op);
	virtual void progressChange(const LangString &op, const float percentage);

protected:
	static ServerConsoleProgressCounter *instance_;
	bool firstOp_;
	int hashes_;
	ProgressCounter progressCounter_;

private:
	ServerConsoleProgressCounter();
	virtual ~ServerConsoleProgressCounter();

};

#endif
