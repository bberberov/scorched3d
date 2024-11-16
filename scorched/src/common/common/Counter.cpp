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

#include <stdlib.h>
#include <common/Counter.h>
#include <common/Defines.h>

Counter::Counter(float minTime, float timeDiff) :
	currentTime_(0.0f), minTime_(minTime), timeDiff_(timeDiff)
{
	genNextTime();
}

bool Counter::nextDraw(float frameTime)
{
	currentTime_ += frameTime;
	if (currentTime_ > nextTime_)
	{
		genNextTime();
		currentTime_ = 0.0f;
		return true;
	}

	return false;
}

void Counter::genNextTime()
{
	nextTime_ = RAND * timeDiff_ + minTime_;
}
