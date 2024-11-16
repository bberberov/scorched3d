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

#include <placement/PlacementObject.h>
#include <placement/PlacementObjectTarget.h>
#include <placement/PlacementObjectGroup.h>
#include <placement/PlacementObjectRandom.h>
#include <placement/PlacementObjectShadow.h>
#include <placement/PlacementObjectNone.h>
#include <placement/PlacementObjectHeight.h>
#include <common/DefinesString.h>
#include <XML/XMLParser.h>

PlacementObject *PlacementObject::create(const char *type)
{
	if (0 == strcmp(type, "model")) return new PlacementObjectTarget;
	if (0 == strcmp(type, "target")) return new PlacementObjectTarget;
	if (0 == strcmp(type, "group")) return new PlacementObjectGroup;
	if (0 == strcmp(type, "random")) return new PlacementObjectRandom;
	if (0 == strcmp(type, "shadow")) return new PlacementObjectShadow;
	if (0 == strcmp(type, "height")) return new PlacementObjectHeight;
	if (0 == strcmp(type, "none")) return new PlacementObjectNone;
	S3D::dialogMessage("PlacementObject", S3D::formatStringBuffer("Unknown object type %s", type));
	return 0;
}

PlacementObject::PlacementObject()
{
}

PlacementObject::~PlacementObject()
{
}

bool PlacementObject::readXML(XMLNode *node)
{
	return node->failChildren();
}
