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

#if !defined(__INCLUDE_EconomyNoneh_INCLUDE__)
#define __INCLUDE_EconomyNoneh_INCLUDE__

#include <weapons/Economy.h>

/**
A very simple economy.
This economy never changes the prices of the weapons.
They are fixed at the price specified in the accessories.xml
file regardless of buys and sells.
*/
class EconomyNone : public Economy
{
public:
	EconomyNone();
	virtual ~EconomyNone();

	virtual bool loadPrices();
	virtual bool savePrices();
	virtual void calculatePrices();

	virtual void accessoryBought(Tank *tank, 
		const char *accessoryName);
	virtual void accessorySold(Tank *tank, 
		const char *accessoryName);

REGISTER_CLASS_HEADER(EconomyNone);
};

#endif
