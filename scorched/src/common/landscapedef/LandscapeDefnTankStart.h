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

#if !defined(__INCLUDE_LandscapeDefnTankStarth_INCLUDE__)
#define __INCLUDE_LandscapeDefnTankStarth_INCLUDE__

#include <landscapedef/LandscapeDefn.h>
#include <common/fixed.h>
#include <image/Image.h>
#include <coms/ComsMessage.h>
#include <XML/XMLFile.h>
#include <string>

class LandscapeDefnTypeTankStart
{
public:
	enum TankStartDefnType
	{
		eHeight,
		ePositional
	};

	static LandscapeDefnTypeTankStart *createType(const char *type);

	virtual bool readXML(XMLNode *node) = 0;
	virtual TankStartDefnType getType() = 0;

	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator) = 0;
};

class LandscapeDefnTankStartHeight : public LandscapeDefnTypeTankStart
{
public:
	virtual bool readXML(XMLNode *node);
	virtual TankStartDefnType getType() { return eHeight; }
	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator);

protected:
	fixed flatness;
	fixed startcloseness;
	fixed heightmin, heightmax;
	std::string startmask;
	Image tankMask;
};

class LandscapeDefnTankStartPositional : public LandscapeDefnTypeTankStart
{
public:
	virtual bool readXML(XMLNode *node);
	virtual TankStartDefnType getType() { return ePositional; }
	virtual FixedVector placeTank(unsigned int playerId, int team,
		ScorchedContext &context, RandomGenerator &generator);

protected:
	fixed flatness;
	fixed heightmin, heightmax;
	std::vector<FixedVector> positions;
};

#endif
