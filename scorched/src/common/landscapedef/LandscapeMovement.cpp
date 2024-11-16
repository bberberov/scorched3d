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

#include <landscapedef/LandscapeMovement.h>
#include <XML/XMLNode.h>
#include <common/Defines.h>

LandscapeMovementType *LandscapeMovementType::create(const char *type)
{
	if (0 == strcmp(type, "boids")) return new LandscapeMovementTypeBoids;
	if (0 == strcmp(type, "ships")) return new LandscapeMovementTypeShips;
	if (0 == strcmp(type, "spline")) return new LandscapeMovementTypeSpline;
	S3D::dialogMessage("LandscapeMovementType", S3D::formatStringBuffer("Unknown movement type %s", type));
	return 0;
}

bool LandscapeMovementType::readXML(XMLNode *node)
{
	if (!node->getNamedChild("groupname", groupname)) return false;

	return node->failChildren();
}

bool LandscapeMovementTypeShips::readXML(XMLNode *node)
{
	if (!node->getNamedChild("speed", speed)) return false;
	if (!node->getNamedChild("controlpoints", controlpoints)) return false;
	if (!node->getNamedChild("controlpointswidth", controlpointswidth)) return false;
	if (!node->getNamedChild("controlpointsheight", controlpointsheight)) return false;
	if (!node->getNamedChild("controlpointsrand", controlpointsrand)) return false;
	if (!node->getNamedChild("starttime", starttime)) return false;

	return LandscapeMovementType::readXML(node);
}

bool LandscapeMovementTypeSpline::readXML(XMLNode *node)
{
	if (!node->getNamedChild("speed", speed)) return false;
	if (!node->getNamedChild("starttime", starttime)) return false;
	if (!node->getNamedChild("groundonly", groundonly)) return false;
	FixedVector point;
	while (node->getNamedChild("controlpoint", point, false))
	{
		points.push_back(point);
	}

	if (points.size() < 3) return node->returnError("Must have at least 3 control points");

	return LandscapeMovementType::readXML(node);
}

bool LandscapeMovementTypeBoids::readXML(XMLNode *node)
{
	if (!node->getNamedChild("minbounds", minbounds)) return false;
	if (!node->getNamedChild("maxbounds", maxbounds)) return false;
	if (!node->getNamedChild("maxvelocity", maxvelocity)) return false;
	if (!node->getNamedChild("cruisedistance", cruisedistance)) return false;
	if (!node->getNamedChild("maxacceleration", maxacceleration)) return false;

	if (maxbounds[0] - minbounds[0] < fixed(25) ||
		maxbounds[1] - minbounds[1] < fixed(25) ||
		maxbounds[2] - minbounds[2] < fixed(10))
	{
		return node->returnError(
			"Boid bounding box is too small, it must be at least 25x10 units");
	}

	return LandscapeMovementType::readXML(node);
}
