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

#if !defined(__INCLUDE_LandscapeMovementh_INCLUDE__)
#define __INCLUDE_LandscapeMovementh_INCLUDE__

#include <vector>
#include <common/FixedVector.h>
#include <common/ModelID.h>

class LandscapeMovementType
{
public:
	enum Type
	{
		eShips,
		eBoids,
		eSpline
	};

	static LandscapeMovementType *create(const char *type);

	virtual bool readXML(XMLNode *node);
	virtual Type getType() = 0;

	std::string groupname;
};

class LandscapeMovementTypeBoids : public LandscapeMovementType
{
public:
	ModelID model;
	FixedVector minbounds, maxbounds;
	fixed maxvelocity;
	fixed cruisedistance;
	fixed maxacceleration;

	virtual bool readXML(XMLNode *node);
	virtual Type getType() { return eBoids; }
};

class LandscapeMovementTypeShips : public LandscapeMovementType
{
public:
	fixed speed;
	int controlpoints;
	fixed controlpointswidth;
	fixed controlpointsheight;
	fixed controlpointsrand;
	fixed starttime;

	virtual bool readXML(XMLNode *node);
	virtual Type getType() { return eShips; }
};

class LandscapeMovementTypeSpline : public LandscapeMovementType
{
public:
	fixed speed;
	fixed starttime;
	bool groundonly;
	std::vector<FixedVector> points;

	virtual bool readXML(XMLNode *node);
	virtual Type getType() { return eSpline; }
};


#endif // __INCLUDE_LandscapeMovementh_INCLUDE__
