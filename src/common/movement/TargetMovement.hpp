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

#ifndef __INCLUDE_TargetMovement_hpp_INCLUDE__
#define __INCLUDE_TargetMovement_hpp_INCLUDE__

#include <landscapedef/LandscapeInclude.hpp>
#include <movement/TargetMovementEntry.hpp>

class ScorchedContext;
class RandomGenerator;
class TargetMovement
{
public:
	TargetMovement();
	virtual ~TargetMovement();

	void generate(ScorchedContext &context);
	void simulate(ScorchedContext &context, fixed frameTime);
	void draw();
	void reset();
	void clear();

protected:
	std::vector<TargetMovementEntry *> movements_;

	void addMovements(ScorchedContext &context, 
		RandomGenerator &random, 
		std::vector<LandscapeInclude *> &movements);
	void addMovementType(ScorchedContext &context, 
		RandomGenerator &random, 
		std::vector<LandscapeMovementType *> &movementtype);
};

#endif // __INCLUDE_TargetMovement_hpp_INCLUDE__
