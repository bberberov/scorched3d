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

#include <movement/TargetMovementEntryShips.h>
#include <common/Defines.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroupEntry.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeMovement.h>

TargetMovementEntryShips::TargetMovementEntryShips()
{
}

TargetMovementEntryShips::~TargetMovementEntryShips()
{
}

void TargetMovementEntryShips::generate(ScorchedContext &context, 
	RandomGenerator &random, LandscapeMovementType *movementType)
{
	int mapWidth = context.getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int mapHeight = context.getLandscapeMaps().getGroundMaps().getLandscapeHeight();
	LandscapeTex &tex = *context.getLandscapeMaps().getDefinitions().getTex();

	// Get the water height (if water is on)
	fixed waterHeight = 0;
	if (tex.border->getType() == LandscapeTexType::eWater)
	{
		LandscapeTexBorderWater *water = 
			(LandscapeTexBorderWater *) tex.border;
       	waterHeight = water->height;
	}

	// Create the spline path the ships will move on
	// Do this from the set of control points specified in the xml file
	LandscapeMovementTypeShips *shipGroup = 
		(LandscapeMovementTypeShips *) movementType;
	std::vector<FixedVector> controlPoints;
	controlPoints.push_back(FixedVector::getNullVector());
	fixed diff = fixed(360) / fixed(shipGroup->controlpoints);
	for (fixed i=0; i<360; i+=diff)
	{
		fixed distWidth = random.getRandFixed("ShipMovement") * 
			shipGroup->controlpointsrand + shipGroup->controlpointswidth;
		fixed distHeight = random.getRandFixed("ShipMovement") * 
			shipGroup->controlpointsrand + shipGroup->controlpointsheight;
		fixed x = (i / 180 * fixed::XPI).sin() * distWidth + fixed(mapWidth) / 2;
		fixed y = (i / 180 * fixed::XPI).cos() * distHeight + fixed(mapHeight) / 2;

		FixedVector pt(x,y,waterHeight);
		controlPoints.push_back(pt);
	}

	// Add a control point at the end to join the loop
	FixedVector midPt = (controlPoints[1] + controlPoints.back()) / 2;
	controlPoints.push_back(midPt);
	controlPoints.front() = midPt;

	// Generate the spline path
	path_.generate(controlPoints, 200, 3, shipGroup->speed);
	path_.simulate(shipGroup->starttime);

	// Find the group to move the objects in
	objectGroup_ = context.getObjectGroups().getGroup(shipGroup->groupname.c_str());
	if (!objectGroup_)
	{
		S3D::dialogExit("TargetMovementEntryShips", 
			S3D::formatStringBuffer("Group entry %s has no objects defined for it", 
			shipGroup->groupname.c_str()));
	}

	// Generate the list of offsets for all of the targets in the group
	ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup_);
	ObjectGroupEntry *entry;
	while (entry = iterator.getNext())
	{
		if (entry->getType() != ObjectGroupEntry::TypeTarget)
		{
			S3D::dialogExit("TargetMovementEntryShips",
				"Movement can be assigned to level targets only (no particles)");
		}

		Target *target = (Target *) entry->getObject();
		if (target->getType() == Target::TypeTank ||
			target->getPlayerId() >= TargetID::MIN_TARGET_TRANSIENT_ID)
		{
			S3D::dialogExit("TargetMovementEntryShips",
				"Movement can be assigned to level targets only (no tanks)");
		}
		if (target->getTargetState().getMovement())
		{
			S3D::dialogExit("TargetMovementEntryBoids",
				"Only one movement can be assigned to each target");
		}

		// Generate the offsets for each target
		fixed offX = random.getRandFixed("ShipMovement") * 200;
		fixed offY = random.getRandFixed("ShipMovement") * 200;
		FixedVector offset(offX, offY - 100, 0);
		TargetMovementEntryShipsOffset *offsetEntry = new TargetMovementEntryShipsOffset();
		offsetEntry->offset = offset;

		// Set this target as moving
		target->getTargetState().setMovement(offsetEntry);
	}
}

void TargetMovementEntryShips::simulate(ScorchedContext &context, fixed frameTime)
{
	// Update the position of all of the ships along the path
	path_.simulate(frameTime);

	// Get the position and direction along the current ship path
	FixedVector position;
	FixedVector direction;
	path_.getPathAttrs(position, direction);
	FixedVector directionPerp = direction.get2DPerp();

	// For each target set position and rotation based on its offset
	ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup_);
	ObjectGroupEntry *entry;
	while (entry = iterator.getNext())
	{
		if (entry->getType() != ObjectGroupEntry::TypeTarget)
		{
			S3D::dialogExit("TargetMovementEntryShips",
				"Movement can be assigned to level targets only (no particles)");
		}

		Target *target = (Target *) entry->getObject();
		TargetMovementEntryShipsOffset *offsetEntry = (TargetMovementEntryShipsOffset *) 
			target->getTargetState().getMovement();
		if (offsetEntry)
		{
			// Calculate position
			FixedVector &offset = offsetEntry->offset;
			FixedVector shipPosition = position;
			shipPosition += directionPerp * -offset[0];
			shipPosition += direction * offset[1];
			fixed angle = atan2x(direction[1], direction[0]);
			fixed angleDegs = (angle / fixed::XPI) * 180 - 90;

			// Update target
			target->getLife().setTargetPositionAndRotation(shipPosition, angleDegs);
		}
	}
}

void TargetMovementEntryShips::reset()
{
}

void TargetMovementEntryShips::draw()
{
#ifndef S3D_SERVER
	path_.draw();
#endif
}
