////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_TanketBatteries_hpp_INCLUDE__
#define __INCLUDE_TanketBatteries_hpp_INCLUDE__

#include <net/NetBuffer.hpp>

class ScorchedContext;
class Tanket;
class Accessory;
class TanketBatteries  
{
public:
	TanketBatteries(ScorchedContext &context);
	virtual ~TanketBatteries();

	void setTanket(Tanket *tanket) { tanket_ = tanket; }

	void newMatch();
	void changed();
	int getNoBatteries();
	Accessory *getBatteryAccessory();
	bool canUse();

protected:
	ScorchedContext &context_;
	Tanket *tanket_;
};

#endif // __INCLUDE_TanketBatteries_hpp_INCLUDE__
