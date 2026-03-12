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

#ifndef __INCLUDE_TankColorGenerator_hpp_INCLUDE__
#define __INCLUDE_TankColorGenerator_hpp_INCLUDE__

#include <map>
#include <vector>
#include <common/Vector.hpp>

class Tank;

// SINGLETON
class TankColorGenerator
{
public:
	static TankColorGenerator* instance();

	Vector& getNextColor( std::map< unsigned int, Tank* >& tanks );

	bool colorAvailable( Vector& color, std::map< unsigned int, Tank* >& tanks, Tank* currentTank = 0 );
	std::vector< Vector* > getAvailableColors( std::map< unsigned int, Tank* >& tanks, Tank* currentTank = 0 );

	std::vector< Vector* >& getAllColors() { return availableColors_; }

	static Vector&     getTeamColor( int team );
	static const char* getTeamName( int team );

protected:
	static TankColorGenerator* instance_;

	std::vector< Vector* > availableColors_;

	void addColor( unsigned int r, unsigned int g, unsigned int b );

private:
	TankColorGenerator();
	virtual ~TankColorGenerator();
};

#endif  // __INCLUDE_TankColorGenerator_hpp_INCLUDE__
