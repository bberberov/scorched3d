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

#include <movement/TargetMovement.h>
#include <movement/TargetMovementEntryShips.h>
#include <movement/TargetMovementEntryBoids.h>
#include <movement/TargetMovementEntrySpline.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeMovement.h>

TargetMovement::TargetMovement()
{
}

TargetMovement::~TargetMovement()
{
	clear();
}

void TargetMovement::clear()
{
	while (!movements_.empty())
	{
		TargetMovementEntry *movement = movements_.back();
		movements_.pop_back();
		delete movement;
	}
}

void TargetMovement::generate(ScorchedContext &context)
{
	clear();

	LandscapeTex &tex = 
		*context.getLandscapeMaps().getDefinitions().getTex();
	LandscapeDefn &defn = 
		*context.getLandscapeMaps().getDefinitions().getDefn();
	FileRandomGenerator random;
	random.seed(context.getLandscapeMaps().getDefinitions().getSeed());

	addMovements(context, random, tex.texDefn.includes);
	addMovements(context, random, defn.texDefn.includes);
}

void TargetMovement::addMovements(ScorchedContext &context, 
	RandomGenerator &random, 
	std::vector<LandscapeInclude *> &movements)
{
	std::vector<LandscapeInclude *>::iterator itor;
	for (itor = movements.begin();
		itor != movements.end();
		++itor)
	{
		LandscapeInclude *movement = (*itor);
		addMovementType(context, random, movement->movements);
	}
}

void TargetMovement::addMovementType(ScorchedContext &context, 
	RandomGenerator &random, 
	std::vector<LandscapeMovementType *> &movementtypes)
{
	std::vector<LandscapeMovementType *>::iterator itor;
	for (itor = movementtypes.begin();
		itor != movementtypes.end();
		++itor)
	{
		LandscapeMovementType *movementtype = (*itor);

		TargetMovementEntry *entry = 0;
		switch(movementtype->getType())
		{
		case LandscapeMovementType::eBoids:
			entry = new TargetMovementEntryBoids();
			break;
		case LandscapeMovementType::eShips:
			entry = new TargetMovementEntryShips();
			break;
		case LandscapeMovementType::eSpline:
			entry = new TargetMovementEntrySpline();
			break;
		default:
			DIALOG_ASSERT(0);
			break;
		}
		entry->generate(context, random, movementtype);
		movements_.push_back(entry);
	}
}

void TargetMovement::simulate(ScorchedContext &context, fixed frameTime)
{
	std::vector<TargetMovementEntry *>::iterator itor;
	for (itor = movements_.begin();
		itor != movements_.end();
		++itor)
	{
		TargetMovementEntry *movementEntry = *itor;
		movementEntry->simulate(context, frameTime * 20);
	}
}

void TargetMovement::draw()
{
	std::vector<TargetMovementEntry *>::iterator itor;
	for (itor = movements_.begin();
		itor != movements_.end();
		++itor)
	{
		TargetMovementEntry *movementEntry = *itor;
		movementEntry->draw();
	}
}

void TargetMovement::reset()
{
	std::vector<TargetMovementEntry *>::iterator itor;
	for (itor = movements_.begin();
		itor != movements_.end();
		++itor)
	{
		TargetMovementEntry *movementEntry = *itor;
		movementEntry->reset();
	}
}
