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

#include <placement/PlacementObjectTarget.hpp>
#include <landscapemap/LandscapeMaps.hpp>
#include <engine/ScorchedContext.hpp>
#include <weapons/AccessoryStore.hpp>
#include <common/Defines.hpp>
#include <target/TargetContainer.hpp>
#include <target/TargetLife.hpp>
#include <target/Target.hpp>
#include <XML/XMLParser.hpp>

PlacementObjectTarget::PlacementObjectTarget()
{
}

PlacementObjectTarget::~PlacementObjectTarget()
{
}

bool PlacementObjectTarget::readXML(XMLNode *node)
{
	if (!targetDef_.readXML(node)) return false;
	if (!groups_.readXML(node, "groupname")) return false;
	return PlacementObject::readXML(node);
}

void PlacementObjectTarget::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Position &position)
{
	++playerId;
	Target *target = targetDef_.createTarget(
		playerId, position.position, position.velocity, context, generator);

	context.getLandscapeMaps().getGroundMaps().getShadows().push_back(
		PlacementShadowDefinition::Entry(
		&targetDef_.getShadow(),
		position.position,
		target->getLife().getSize()));
}
