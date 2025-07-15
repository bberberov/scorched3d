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


// NumberParser.h: interface for the NumberParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
#define AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_

#include <string>
#include <list>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>

class NumberParser
{
public:
	NumberParser(const char *valueName);
	NumberParser(const char *valueName, fixed value);
	virtual ~NumberParser();

	fixed getValue(ScorchedContext &context); 
	fixed getValue(ScorchedContext &context, fixed defaultValue);
	int getInt(ScorchedContext &context);
	unsigned int getUInt(ScorchedContext &context);
	bool setExpression(const char *expression);
	bool setExpression(fixed value);

protected:
	const char *valueName_;
	bool getOperands();
	bool isFixed_, isValid_;
	std::string expression_;
	std::list<fixed> operands_;
	fixed max_, min_, step_;

};

#endif // !defined(AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
