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

#if !defined(__INCLUDE_EconomyFreeMarketh_INCLUDE__)
#define __INCLUDE_EconomyFreeMarketh_INCLUDE__

#include <weapons/Economy.h>
#include <map>

/**
This economy changes the prices of the weapons depending
on how many have been bought.  The more popular accessories
rise in price and the less popular accessories fall.
Thus making people choose different weapons from time to time
(perhaps).
*/
class Accessory;
class EconomyFreeMarket : public Economy
{
public:
	EconomyFreeMarket();
	virtual ~EconomyFreeMarket();

	virtual bool loadPrices();
	virtual bool savePrices();
	virtual void calculatePrices();

	virtual void accessoryBought(Tank *tank, 
		const char *accessoryName);
	virtual void accessorySold(Tank *tank, 
		const char *accessoryName);

REGISTER_CLASS_HEADER(EconomyFreeMarket);

protected:
	std::map<unsigned int, float> accessoryPrice_;

	void setPrice(Accessory *accessory, int price);
};

#endif
