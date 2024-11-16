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

#include <movement/TargetMovementEntrySpline.h>
#include <common/Defines.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeMovement.h>

TargetMovementEntrySpline::TargetMovementEntrySpline() : context_(0)
{
}

TargetMovementEntrySpline::~TargetMovementEntrySpline()
{
}

void TargetMovementEntrySpline::generate(ScorchedContext &context, 
	RandomGenerator &random, LandscapeMovementType *movementType)
{
	// Create the spline path the target will move on
	// Do this from the set of control points specified in the xml file
	LandscapeMovementTypeSpline *splineGroup = 
		(LandscapeMovementTypeSpline *) movementType;
	context_ = &context;
	groundOnly_ = splineGroup->groundonly;

	// Create the control points from those specified
	std::vector<FixedVector> controlPoints;
	controlPoints.push_back(FixedVector::getNullVector());
	controlPoints.insert(controlPoints.end(), splineGroup->points.begin(), splineGroup->points.end());

	// Add a control point at the end to join the loop
	FixedVector midPt = (controlPoints[1] + controlPoints.back()) / 2;
	controlPoints.push_back(midPt);
	controlPoints.front() = midPt;

	// This is done mainly so they are draw correctly for debug
	if (groundOnly_) 
	{
		std::vector<FixedVector>::iterator itor;
		for (itor = controlPoints.begin();
			itor != controlPoints.end();
			++itor)
		{
			FixedVector &point = (*itor);
			point[2] = context.getLandscapeMaps().getGroundMaps().getInterpHeight(
				point[0], point[1]);
		}
	}

	// Generate the spline path
	path_.generate(controlPoints, 200, 3, splineGroup->speed);
	path_.simulate(splineGroup->starttime);

	// Find the group to move the objects in
	objectGroup_ = context.getObjectGroups().getGroup(splineGroup->groupname.c_str());
	if (!objectGroup_)
	{
		S3D::dialogExit("TargetMovementEntrySpline", 
			S3D::formatStringBuffer("Group entry %s has no objects defined for it", 
			splineGroup->groupname.c_str()));
	}

	// Generate the list of offsets for all of the targets in the group
	ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup_);
	ObjectGroupEntry *entry;
	while (entry = iterator.getNext())
	{
		makeObject(entry);
	}
}

void TargetMovementEntrySpline::makeObject(ObjectGroupEntry *entry)
{
	if (entry->getType() != ObjectGroupEntry::TypeTarget)
	{
		S3D::dialogExit("TargetMovementEntrySpline",
			"Movement can be assigned to level targets only (no particles)");
	}

	Target *target = (Target *) entry->getObject();
	if (target->getType() == Target::TypeTank ||
		target->getPlayerId() >= TargetID::MIN_TARGET_TRANSIENT_ID)
	{
		S3D::dialogExit("TargetMovementEntrySpline",
			"Movement can be assigned to level targets only (no tanks)");
	}
	if (target->getTargetState().getMovement())
	{
		S3D::dialogExit("TargetMovementEntrySpline",
			"Only one movement can be assigned to each target");
	}

	// Set this target as moving
	target->getTargetState().setMovement(new TargetStateMovement());
}

void TargetMovementEntrySpline::simulate(ScorchedContext &context, fixed frameTime)
{
	// Update the position of all of the targets along the path
	path_.simulate(frameTime);

	// Get the position and direction along the current path
	FixedVector position;
	FixedVector direction;
	path_.getPathAttrs(position, direction);
	FixedVector directionPerp = direction.get2DPerp();

	// Move the position to the ground if set
	if (groundOnly_)
	{
		position[2] = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]);
	}

	// For each target set position and rotation based on its offset
	ObjectGroup::ObjectGroupEntryHolderIterator iterator(objectGroup_);
	ObjectGroupEntry *entry;
	while (entry = iterator.getNext())
	{
		if (entry->getType() != ObjectGroupEntry::TypeTarget)
		{
			S3D::dialogExit("TargetMovementEntrySpline",
				"Movement can be assigned to level targets only (no particles)");
		}
		
		Target *target = (Target *) entry->getObject();
		if (!target->getTargetState().getMovement()) makeObject(entry);

		fixed angle = atan2x(direction[1], direction[0]);
		fixed angleDegs = (angle / fixed::XPI) * 180 - 90;

		// Update target
		target->getLife().setTargetPositionAndRotation(
			position, angleDegs);
		target->getLife().setVelocity(direction);
	}
}

void TargetMovementEntrySpline::reset()
{
}

void TargetMovementEntrySpline::draw()
{
#ifndef S3D_SERVER
	path_.draw();
#endif
}
