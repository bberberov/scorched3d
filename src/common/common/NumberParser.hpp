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

#ifndef __INCLUDE_NumberParser_hpp_INCLUDE__
#define __INCLUDE_NumberParser_hpp_INCLUDE__

// NumberParser.hpp: interface for the NumberParser class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <common/RandomGenerator.hpp>
#include <engine/ScorchedContext.hpp>

class NumberParser
{
public:
	NumberParser( const char* valueName );
	NumberParser( const char* valueName, fixed value );
	virtual ~NumberParser();

	fixed        getValue( ScorchedContext& context );
	fixed        getValue( ScorchedContext& context, fixed defaultValue );
	int          getInt( ScorchedContext& context );
	unsigned int getUInt( ScorchedContext& context );
	bool         setExpression( const char* expression );
	bool         setExpression( fixed value );

protected:
	const char*        valueName_;
	bool               getOperands();
	bool               isFixed_, isValid_;
	std::string        expression_;
	std::list< fixed > operands_;
	fixed              max_, min_, step_;
};

#endif  // __INCLUDE_NumberParser_hpp_INCLUDE__
