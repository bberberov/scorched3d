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

#include <landscapemap/MovementMap.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <weapons/WeaponMoveTank.h>
#include <engine/ScorchedContext.h>
#include <common/FixedVector.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <tanket/Tanket.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <image/ImageFactory.h>
#include <memory.h>

MovementMap::MovementMap(
	Tanket *tanket, 
	ScorchedContext &context) :
	tanket_(tanket), context_(context)
{
	arenaX_ = context_.getLandscapeMaps().getGroundMaps().getArenaX();
	arenaY_ = context_.getLandscapeMaps().getGroundMaps().getArenaY();
	arenaWidth_ = context_.getLandscapeMaps().getGroundMaps().getArenaWidth();
	arenaHeight_ = context_.getLandscapeMaps().getGroundMaps().getArenaHeight();
	landscapeWidth_ = context_.getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	landscapeHeight_ = context_.getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	// Create the empty movement map
	entries_ = new MovementMapEntry[(landscapeWidth_ + 1) * (landscapeHeight_ + 1)];
	memset(entries_, (int) eNotInitialized, sizeof(MovementMapEntry) * (landscapeWidth_ + 1) * (landscapeHeight_ + 1));

	// Get the minimum height we are allowed to move to
	minHeight_ = getWaterHeight();

	// Generate a list of all of the targets we need to check
	// to see if we are in their shields
	std::map<unsigned int, Target *>::iterator targetItor;
	std::map<unsigned int, Target *> &targets = 
		context.getTargetContainer().getTargets();
	for (targetItor = targets.begin(); 
		targetItor != targets.end();
		++targetItor)
	{
		Target *target = (*targetItor).second;
		if (movementProof(context, target, tanket))
		{
			checkTargets_.push_back(target);
		}
	}
}

MovementMap::~MovementMap()
{
	delete [] entries_;
	entries_ = 0;
}

unsigned int MovementMap::POINT_TO_UINT(unsigned int x, unsigned int y)
{
	return (x << 16) | (y & 0xffff);
}

MovementMap::MovementMapEntry &MovementMap::getEntry(int w, int h)
{ 
	if (w>=arenaX_ && h>=arenaY_ && w<=arenaX_ + arenaWidth_ && h<=arenaY_ + arenaHeight_)
	{
		MovementMapEntry &entry = entries_[(landscapeWidth_+1) * h + w];
		return entry;
	}
	static MovementMapEntry entry(eNoMovement, fixed(1000), 0, 0);
	entry.type = eNoMovement;

	return entry;
}

MovementMap::MovementMapEntry &MovementMap::getAndCheckEntry(int w, int h)
{
	MovementMapEntry &entry = getEntry(w, h);

	// This entry has not been checked
	if (entry.type == eNotInitialized)
	{
		fixed height = 
			context_.getLandscapeMaps().getGroundMaps().getHeight(w, h);
		FixedVector position(w, h, height);
		entry.type = eNotSeen;

		// Check if this position is below water level
		if (height < minHeight_)
		{
			entry.type = eNoMovement;
		}
		else
		{
			// Check if this position is inside any shields
			std::list<Target *>::iterator targetItor;
			for (targetItor = checkTargets_.begin(); 
				targetItor != checkTargets_.end();
				++targetItor)
			{
				Target *target = (*targetItor);
				if (inShield(target, tanket_, position))
				{
					entry.type = eNoMovement;
					break;
				}
			}
		}
	}
	return entry;
}

void MovementMap::addPoint(unsigned int x, unsigned int y, 
					 fixed height, fixed dist,
					 std::list<unsigned int> &edgeList,
					 unsigned int sourcePt,
					 unsigned int epoc)
{
	// Check that we are not going outside the arena
	if (x < (unsigned int) (arenaX_ + 5) || 
		y < (unsigned int) (arenaY_ + 5) ||
		x > (unsigned int) (arenaX_ + arenaWidth_ - 5) ||
		y > (unsigned int) (arenaY_ + arenaHeight_ - 5)) return;

	// Check if we can already reach this point
	// Through a shorted already visited path
	// That is not a current edge point
	MovementMapEntry &priorEntry = getAndCheckEntry(x, y);

	if (priorEntry.type == eNoMovement)
	{
		return;
	}
	if (priorEntry.type == eMovement &&
		priorEntry.epoc < epoc)
	{
		return;
	}

	// Find how much the tank has to climb to reach this new point
	// check that this is acceptable
	fixed newHeight = context_.getLandscapeMaps().getGroundMaps().getHeight(
		x, y);

	// Check water height 
	if (newHeight < minHeight_) 
	{
		return; 
	}

	// Check climing height
	fixed MaxTankClimbHeight = fixed(context_.getOptionsGame().
		getMaxClimbingDistance()) / fixed(10);
	if (newHeight - height > MaxTankClimbHeight) 
	{
		return;
	}

	// Check if we can also reach this point from another edge point
	if (priorEntry.epoc == epoc)
	{
		// Check if this prior edge is further than the current
		if (dist < priorEntry.dist)
		{
			// If so set the distance etc
			priorEntry = MovementMap::MovementMapEntry(
				MovementMap::eMovement,
				dist,
				sourcePt,
				epoc);
		}
	}
	else
	{
		// Add this new edge to the list of edges
		unsigned int pt = POINT_TO_UINT(x, y);

		// Set the distance etc
		edgeList.push_back(pt);
		priorEntry = MovementMap::MovementMapEntry(
			MovementMap::eMovement,
			dist,
			sourcePt,
			epoc);
	}
}

bool MovementMap::movementProof(ScorchedContext &context, Target *target, Tanket *tanket)
{
	if (!target->getAlive() ||
		!target->getShield().getCurrentShield()) return false;

	Shield *shield = (Shield *)
		(target->getShield().getCurrentShield()->getAction());

	bool movementProof = true;
	switch (shield->getMovementProof())
	{
	case Shield::ShieldMovementAll:
		movementProof = false;
		break;
	case Shield::ShieldMovementNone:
		movementProof = true;
		break;
	case Shield::ShieldMovementSame:
		if (target->getPlayerId() == tanket->getPlayerId())
		{
			movementProof = false;
		}
		else if (context.getOptionsGame().getTeams() > 1 &&
			target->getType() != Target::TypeTarget)
		{
			Tanket *targetTanket = (Tanket *) target;
			if (targetTanket->getTeam() == tanket->getTeam())
			{
				movementProof = false;
			}
		}
		break;
	case Shield::ShieldMovementTeam1:
		if (tanket->getTeam() == 1 ||
			tanket->getTeam() == 0) movementProof = false;
		break;
	case Shield::ShieldMovementTeam2:
		if (tanket->getTeam() == 2 ||
			tanket->getTeam() == 0) movementProof = false;
		break;
	case Shield::ShieldMovementTeam3:
		if (tanket->getTeam() == 3 ||
			tanket->getTeam() == 0) movementProof = false;
		break;
	case Shield::ShieldMovementTeam4:
		if (tanket->getTeam() == 4 ||
			tanket->getTeam() == 0) movementProof = false;
		break;
	}
	return movementProof;
}

bool MovementMap::inShield(Target *target, Tanket *tanket, FixedVector &position)
{
	Shield *shield = (Shield *)
		(target->getShield().getCurrentShield()->getAction());

	FixedVector offset = position - target->getLife().getTargetPosition();
	offset[0] = offset[0].abs();
	offset[1] = offset[1].abs();
	offset[2] = fixed(0);
	FixedVector surround = offset.Normalize() * fixed(2);
	offset[0] = MAX(fixed(0), offset[0] - surround[0]);
	offset[1] = MAX(fixed(0), offset[1] - surround[1]);

	return shield->inShield(offset);
}

bool MovementMap::allowedPosition(ScorchedContext &context, Tanket *tanket, FixedVector &position)
{
	std::map<unsigned int, Target *>::iterator targetItor;
	std::map<unsigned int, Target *> targets;
	context.getTargetSpace().getCollisionSet(position, fixed(1), targets);
	for (targetItor = targets.begin(); 
		targetItor != targets.end();
		++targetItor)
	{
		Target *target = (*targetItor).second;

		if (movementProof(context, target, tanket) &&
			inShield(target, tanket, position))
		{
			return false;
		}
	}
	return true;
}

fixed MovementMap::getWaterHeight()
{
	// Calculate the water height
	fixed waterHeight = (-10);
	if (context_.getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLand ||
		context_.getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLandOrAbove)
	{
		LandscapeTex &tex = *context_.getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;

			waterHeight = water->height;
		}
	}

	if (context_.getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLandOrAbove)
	{
		if (waterHeight > tanket_->getLife().getTargetPosition()[2] - fixed(true, 1000))
		{
			waterHeight = tanket_->getLife().getTargetPosition()[2] - fixed(true, 1000);
		}
	}
	return waterHeight;
}

fixed MovementMap::getFuel(WeaponMoveTank *weapon)
{
	fixed fuel = fixed(0);
	int numberFuel = tanket_->getAccessories().getAccessoryCount(weapon->getParent());
	if (numberFuel == -1)
	{
		fuel = weapon->getMaximumRange();
	}
	else
	{
		fuel = MIN(weapon->getMaximumRange(), numberFuel);
	}
	return fuel;
}

bool MovementMap::tankBurried()
{
	fixed landscapeHeight = context_.getLandscapeMaps().getGroundMaps().getInterpHeight(
		tanket_->getLife().getTargetPosition()[0],
		tanket_->getLife().getTargetPosition()[1]);
	fixed tankHeight = 
		tanket_->getLife().getTargetPosition()[2];
	fixed MaxTankClimbHeight = fixed(context_.getOptionsGame().
		getMaxClimbingDistance()) / fixed(10);
	if (landscapeHeight > tankHeight + MaxTankClimbHeight)
	{
		return true;
	}
	return false;
}

bool MovementMap::QueuePosition::operator<(const QueuePosition &rhs) const
{
	return fixed(distance) > fixed(rhs.distance);
}

fixed inline calcDistance(unsigned int pt, FixedVector &position)
{
	unsigned int x = pt >> 16;
	unsigned int y = pt & 0xffff;
	FixedVector pos1((int) x, (int) y, 0);
	FixedVector pos2(position[0], position[1], 0);
	return (pos1-pos2).Magnitude();
}

void MovementMap::addPoint(unsigned int x, unsigned int y, 
	 fixed height, fixed dist,
	 std::priority_queue<QueuePosition, 
		std::vector<QueuePosition>, 
		std::less<QueuePosition> > &priorityQueue,
	 unsigned int sourcePt,
	 FixedVector &position)
{
	// Check that we are not going outside the arena
	if (x < (unsigned int) (arenaX_ + 5) || 
		y < (unsigned int) (arenaY_ + 5) ||
		x > (unsigned int) (arenaX_ + arenaWidth_ - 5) ||
		y > (unsigned int) (arenaY_ + arenaHeight_ - 5)) return;

	// Check if we can already reach this point
	// through a shorter already visited path
	MovementMapEntry &priorEntry = getAndCheckEntry(x, y);

	if (priorEntry.type == eNoMovement ||
		priorEntry.type == eMovement) return;

	// Find how much the tank has to climb to reach this new point
	// check that this is acceptable
	fixed newHeight = context_.getLandscapeMaps().getGroundMaps().getHeight(x, y);

	// Check water height 
	if (newHeight < minHeight_) return; 

	// Check climing height
	fixed MaxTankClimbHeight = fixed(context_.getOptionsGame().
		getMaxClimbingDistance()) / fixed(10);
	if (newHeight - height > MaxTankClimbHeight) return;

	// Add this new edge to the list of edges
	unsigned int pt = POINT_TO_UINT(x, y);
	QueuePosition queuePosition;
	queuePosition.square = pt;
	queuePosition.distance = calcDistance(pt, position) + dist;
	priorityQueue.push(queuePosition);

	// Set the distance etc
	priorEntry = MovementMap::MovementMapEntry(
		MovementMap::eMovement,
		dist,
		sourcePt,
		0);
}

bool MovementMap::calculatePosition(FixedVector &position, fixed fuel)
{
	// Check if the tank is buried and cannot move
	if (tankBurried()) return false;

	// A very clever weighted queue thingy
	std::priority_queue<QueuePosition, 
		std::vector<QueuePosition>, 
		std::less<QueuePosition> > priorityQueue;

	// Setup movement variables
	unsigned int posX = (unsigned int) 
		tanket_->getLife().getTargetPosition()[0].asInt();
	unsigned int posY = (unsigned int) 
		tanket_->getLife().getTargetPosition()[1].asInt();
	unsigned int startPt = POINT_TO_UINT(posX, posY);
	unsigned int endPt = POINT_TO_UINT(
		(unsigned int)position[0].asInt(), 
		(unsigned int)position[1].asInt());

	// Check we can move at all
	if (getAndCheckEntry(posX, posY).type == eNotSeen)
	{
		// Add this point to the movement map
		getEntry(posX, posY) = 
			MovementMap::MovementMapEntry(
				MovementMap::eMovement,
				fixed(0),
				0,
				0);

		// And add it to the list of next edge points
		QueuePosition queuePosition;
		queuePosition.square = startPt;
		queuePosition.distance = calcDistance(startPt, position);
		priorityQueue.push(queuePosition);
	}

	// Itterate
	while (!priorityQueue.empty())
	{
		QueuePosition queuePosition = priorityQueue.top();
		priorityQueue.pop();

		unsigned int pt = queuePosition.square;
		unsigned int x = pt >> 16;
		unsigned int y = pt & 0xffff;

		if (pt == endPt) return true;

		MovementMapEntry &priorEntry = getAndCheckEntry(x, y);
		fixed dist = priorEntry.dist;
		if (dist <= fuel)
		{
			fixed height = 
				context_.getLandscapeMaps().getGroundMaps().getHeight(x, y);

			addPoint(x+1, y, height, dist + 1, priorityQueue, pt, position);
			addPoint(x, y+1, height, dist + 1, priorityQueue, pt, position);
			addPoint(x-1, y, height, dist + 1, priorityQueue, pt, position);
			addPoint(x, y-1, height, dist + 1, priorityQueue, pt, position);
			addPoint(x+1, y+1, height, dist + fixed(true, 14000), priorityQueue, pt, position);
			addPoint(x-1, y+1, height, dist + fixed(true, 14000), priorityQueue, pt, position);
			addPoint(x-1, y-1, height, dist + fixed(true, 14000), priorityQueue, pt, position);
			addPoint(x+1, y-1, height, dist + fixed(true, 14000), priorityQueue, pt, position);
		}
	}

	return false;
}

void MovementMap::calculateAllPositions(fixed fuel)
{
	// Check if the tank is buried and cannot move
	if (tankBurried()) return;

	std::list<unsigned int> edgeList;
	unsigned int epoc = 0;

	// Setup movement variables
	unsigned int posX = (unsigned int) 
		tanket_->getLife().getTargetPosition()[0].asInt();
	unsigned int posY = (unsigned int) 
		tanket_->getLife().getTargetPosition()[1].asInt();

	// Check we can move at all
	if (getAndCheckEntry(posX, posY).type == eNotSeen)
	{
		// Add this point to the movement map
		getEntry(posX, posY) = 
			MovementMap::MovementMapEntry(
				MovementMap::eMovement,
				fixed(0),
				0,
				epoc);

		// And add it to the list of next edge points
		unsigned int pt = POINT_TO_UINT(posX, posY);
		edgeList.push_back(pt);
	}

	// Find all the edges for the current edges and so on
	while (!edgeList.empty())
	{
		epoc++;

		std::list<unsigned int> tmpEdgeList = edgeList;
		edgeList.clear();

		std::list<unsigned int>::iterator edgeItor;
		for (edgeItor = tmpEdgeList.begin();
			edgeItor != tmpEdgeList.end();
			++edgeItor)
		{
			unsigned int pt = (*edgeItor);
			unsigned int x = pt >> 16;
			unsigned int y = pt & 0xffff;

			MovementMapEntry &priorEntry = getAndCheckEntry(x, y);
			fixed dist = priorEntry.dist;
			if (dist <= fuel)
			{
				fixed height = 
					context_.getLandscapeMaps().getGroundMaps().getHeight(x, y);

				addPoint(x+1, y, height, dist + 1, edgeList, pt, epoc);
				addPoint(x, y+1, height, dist + 1, edgeList, pt, epoc);
				addPoint(x-1, y, height, dist + 1, edgeList, pt, epoc);
				addPoint(x, y-1, height, dist + 1, edgeList, pt, epoc);
				addPoint(x+1, y+1, height, dist + fixed(true, 14000), edgeList, pt, epoc);
				addPoint(x-1, y+1, height, dist + fixed(true, 14000), edgeList, pt, epoc);
				addPoint(x-1, y-1, height, dist + fixed(true, 14000), edgeList, pt, epoc);
				addPoint(x+1, y-1, height, dist + fixed(true, 14000), edgeList, pt, epoc);
			}
		}
	}
}

#ifndef S3D_SERVER
#include <landscape/Landscape.h>
#include <client/ScorchedClient.h>

void MovementMap::movementTexture()
{
	Image newMap(
		Landscape::instance()->getMainMap().getWidth(),
		Landscape::instance()->getMainMap().getHeight());

	float landscapeWidth = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	float landscpeHeight = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	GLubyte *src = Landscape::instance()->getMainMap().getBits();
	GLubyte *dest = newMap.getBits();
	for (int y=0; y<newMap.getHeight(); y++)
	{
		int y1 = y + 1;
		if (y1 == newMap.getHeight()) y1--;

		int posY = int(float(y) / float(newMap.getHeight()) * landscpeHeight);
		int posY1 = int(float(y1) / float(newMap.getHeight()) * landscpeHeight);
		for (int x=0; x<newMap.getWidth(); x++)
		{
			int x1 = x + 1;
			if (x1 == newMap.getWidth()) x1--;

			int posX = int(float(x) / float(newMap.getWidth()) * landscapeWidth);
			int posX1 = int(float(x1) / float(newMap.getWidth()) * landscapeWidth);

			MovementMapEntryType type1 = getEntry(posX1, posY).type;
			MovementMapEntryType type2 = getEntry(posX, posY).type;
			MovementMapEntryType type3 = getEntry(posX, posY1).type;

			if ((type1 == eMovement && type2 != eMovement) || 
				(type1 != eMovement && type2 == eMovement) ||
				(type3 == eMovement && type2 != eMovement) || 
				(type3 != eMovement && type2 == eMovement))
			{
				dest[0] = 255;
				dest[1] = 0;
				dest[2] = 0;
			}
			else if (type2 != eMovement)
			{
				dest[0] = src[0] / 4;
				dest[1] = src[1] / 4;
				dest[2] = src[2] / 4;
			}
			else
			{
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
			}

			src+=3;
			dest+=3;
		}
	}

	Landscape::instance()->getMainTexture().replace(newMap, false);
	Landscape::instance()->setTextureType(Landscape::eOther);
}

void MovementMap::limitTexture(FixedVector &center, int limit)
{
	Image newMap(
		Landscape::instance()->getMainMap().getWidth(),
		Landscape::instance()->getMainMap().getHeight());

	float landscapeWidth = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	float landscpeHeight = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	int arenaX = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaHeight();

	GLubyte *src = Landscape::instance()->getMainMap().getBits();
	GLubyte *dest = newMap.getBits();
	for (int y=0; y<newMap.getHeight(); y++)
	{
		int y1 = y + 1;
		if (y1 == newMap.getHeight()) y1--;

		int posY = int(float(y) / float(newMap.getHeight()) * landscpeHeight);
		int posY1 = int(float(y1) / float(newMap.getHeight()) * landscpeHeight);
		for (int x=0; x<newMap.getWidth(); x++)
		{
			int x1 = x + 1;
			if (x1 == newMap.getWidth()) x1--;

			int posX = int(float(x) / float(newMap.getWidth()) * landscapeWidth);
			int posX1 = int(float(x1) / float(newMap.getWidth()) * landscapeWidth);

			bool in1 = false;
			bool in2 = false;
			bool in3 = false;
			if (posX > arenaX &&
				posY > arenaY &&
				posX < arenaX + arenaWidth &&
				posY < arenaY + arenaHeight)
			{
				FixedVector position1(posX1, posY, 0);
				FixedVector position2(posX, posY, 0);
				FixedVector position3(posX, posY1, 0);

				in1 = (position1 - center).Magnitude() < limit;
				in2 = (position2 - center).Magnitude() < limit;
				in3 = (position3 - center).Magnitude() < limit;
			}

			if (in1 != in2 || in2 != in3)
			{
				dest[0] = 255;
				dest[1] = 0;
				dest[2] = 0;
			}
			else if (!in2)
			{
				dest[0] = src[0] / 4;
				dest[1] = src[1] / 4;
				dest[2] = src[2] / 4;
			}
			else
			{
				dest[0] = src[0];
				dest[1] = src[1];
				dest[2] = src[2];
			}

			src+=3;
			dest+=3;
		}
	}

	Landscape::instance()->getMainTexture().replace(newMap, false);
	Landscape::instance()->setTextureType(Landscape::eOther);
}

#endif // #ifndef S3D_SERVER
