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
#include <common/Clock.h>
#include <SDL/SDL.h>

Clock::Clock() 
{
	dwLastTime_ = SDL_GetTicks();
}

Clock::~Clock()
{
}

unsigned int Clock::getTicksDifference()
{
	unsigned int  dwCurrentTime = SDL_GetTicks();
	unsigned int  dwTimeDiff = 1;
	if (dwCurrentTime >= dwLastTime_)
	{
		dwTimeDiff = dwCurrentTime - dwLastTime_;
	}

	dwLastTime_ = dwCurrentTime;
	return dwTimeDiff;
}

float Clock::getTimeDifference()
{
	unsigned int  dwTimeDiff = getTicksDifference();
	return ((float) dwTimeDiff) / 1000.0f;
}

