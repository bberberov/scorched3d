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

#include <target/TargetSpace.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <weapons/Accessory.h>
#include <weapons/ShieldRound.h>
#include <weapons/ShieldSquare.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

TargetSpace::TargetSpace() :
	spaceX_(-128), spaceY_(-128),
	spaceW_(1024), spaceH_(1024),
	spaceSq_(4), context_(0)
{
	spaceWSq_ = (spaceW_ / spaceSq_);
	spaceHSq_ = (spaceH_ / spaceSq_);
	noSquares_ = spaceWSq_ * spaceHSq_;
	squares_ = new Square[noSquares_];
	for (int i=0; i<noSquares_; i++) squares_[i].squarenum = i;
}

TargetSpace::~TargetSpace()
{
	delete [] squares_;
}

void TargetSpace::clear()
{
	for (int i=0; i<noSquares_; i++) squares_[i].targets.clear();
}

void TargetSpace::updateTarget(Target *target)
{
	if (!target->getAlive() ||
		target->getTargetState().getNoCollision())
	{
		removeTarget(target);
	}
	else
	{
		static std::vector<Square*> squares;
		squares.clear();
		getSquares(target, squares);

		bool same = false;
		if (squares.size() == target->getLife().getSpaceContainment().squares.size())
		{
			same = true;
			for (unsigned int i=0; i<squares.size(); i++)
			{
				if (squares[i]->squarenum != target->getLife().getSpaceContainment().squares[i])
				{
					same = false;
					break;
				}
			}
		}

		if (!same)
		{
			removeTarget(target);
			std::vector<Square*>::iterator itor;
			for (itor = squares.begin();
				itor != squares.end();
				++itor)
			{
				Square *square = *itor;
				target->getLife().getSpaceContainment().squares.push_back(square->squarenum);
				square->targets.insert(std::pair<unsigned int, Target*>(target->getPlayerId(), target));
			}
		}
	}
}

void TargetSpace::getSquares(Target *target, std::vector<Square*> &squares)
{
	// Set the bounding constaints
	int x = target->getLife().getTargetPosition()[0].asInt();
	int y = target->getLife().getTargetPosition()[1].asInt();
	int w = target->getLife().getAabbSize()[0].asInt();
	int h = target->getLife().getAabbSize()[1].asInt();

	// Update bounding constraints for shield
	Accessory *shieldAcc = target->getShield().getCurrentShield();
	if (shieldAcc)
	{
		Shield *shield = (Shield *) shieldAcc->getAction();
		if (shield->getRound())
		{
			ShieldRound *round = (ShieldRound *) shield;

			w = MAX(w, (round->getActualRadius() * 2).asInt());
			h = MAX(h, (round->getActualRadius() * 2).asInt());
		}
		else
		{
			ShieldSquare *square = (ShieldSquare *) shield;

			w = MAX(w, (square->getSize()[0] * 2).asInt());
			h = MAX(h, (square->getSize()[1] * 2).asInt());
		}
	}

	// Find the bounding box coords
	int halfW = w / 2;
	int halfH = h / 2;
	int minX = x - halfW - 1;
	int minY = y - halfH - 1;
	int maxX = x + halfW + 1;
	int maxY = y + halfH + 1;

	// Find the square positions
	normalizeCoords(minX, minY);
	normalizeCoords(maxX, maxY);

	// Find the square numbers
	for (int b=minY; b<=maxY; b++)
	{
		for (int a=minX; a<=maxX; a++)
		{
			int num = spaceHSq_ * b + a;
			DIALOG_ASSERT(num >= 0 && num < noSquares_);

			Square *square = &squares_[num];
			squares.push_back(square);
		}
	}
}

void TargetSpace::removeTarget(Target *target)
{
	std::vector<int> &squares = target->getLife().getSpaceContainment().squares;
	while (!squares.empty())
	{
		int squareNum = squares.back();
		squares.pop_back();

		Square *square = &squares_[squareNum];
		square->targets.erase(target->getPlayerId());
	}
}

Target *TargetSpace::getCollision(FixedVector &position)
{
	int x = position[0].asInt();
	int y = position[1].asInt();

	// Find the square positions
	normalizeCoords(x, y);

	int num = spaceHSq_ * y + x;
	DIALOG_ASSERT(num >= 0 && num < noSquares_);

	Square *square = &squares_[num];

	Target *result = 0;
	std::map<unsigned int, Target *> &potentialTargets = square->targets;
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = potentialTargets.begin();
		itor != potentialTargets.end();
		++itor)
	{
		Target *target = (*itor).second;
		if (!target->getAlive())
		{
			Logger::log(S3D::formatStringBuffer("ERROR: Dead target %u:%s in space",
				target->getPlayerId(), target->getCStrName().c_str()));
			continue;
		}

		Accessory *shieldAcc = target->getShield().getCurrentShield();
		if (shieldAcc)
		{
			Shield *shield = (Shield *) shieldAcc->getAction();
			FixedVector direction = position - target->getLife().getTargetPosition();
			if (shield->inShield(direction))
			{
				result = target;
				break;
			}
		}

		if (target->getLife().collision(position)) 
		{
			result = target;
			break;
		}
	}

	if (context_->getOptionsGame().getActionSyncCheck() &&
		context_->getOptionsGame().getActionCollisionSyncCheck())
	{
		std::string targets;
		if (result)
		{
			targets.append(S3D::formatStringBuffer("%u:%s ", 
				result->getPlayerId(),
				result->getLife().getTargetPosition().asQuickString()));
		}

		context_->getSimulator().addSyncCheck(
			S3D::formatStringBuffer("CollisionSet : %s %s", 
				position.asQuickString(),
				targets.c_str()));
	}

	return result;
}

void TargetSpace::getCollisionSet(FixedVector &position, fixed radius, 
	std::map<unsigned int, Target *> &collisionTargets, 
	bool ignoreHeight)
{
	int x = position[0].asInt();
	int y = position[1].asInt();
	int w = radius.asInt();
	int h = radius.asInt();

	// Find the bounding box coords
	int halfW = w; // Not need 1/2 as its the radius thats passed in
	int halfH = h;
	int minX = x - halfW - 1;
	int minY = y - halfH - 1;
	int maxX = x + halfW + 1;
	int maxY = y + halfH + 1;

	// Find the square positions
	normalizeCoords(minX, minY);
	normalizeCoords(maxX, maxY);

	// Find the square numbers
	std::map<unsigned int, Target *>::iterator itor;
	for (int b=minY; b<=maxY; b++)
	{
		for (int a=minX; a<=maxX; a++)
		{
			int num = spaceHSq_ * b + a;
			DIALOG_ASSERT(num >= 0 && num < noSquares_);

			Square *square = &squares_[num];

			std::map<unsigned int, Target *> &potentialTargets =
				square->targets;
			for (itor = potentialTargets.begin();
				itor != potentialTargets.end();
				++itor)
			{
				Target *target = (*itor).second;
				if (!target->getAlive())
				{
					Logger::log(S3D::formatStringBuffer("ERROR: Dead target %u:%s in space",
						target->getPlayerId(), target->getCStrName().c_str()));
					continue;
				}

				FixedVector checkPos = position;
				if (ignoreHeight)
				{
					FixedVector centerPos = target->getLife().getCenterPosition();
					checkPos[2] = centerPos[2];
				}
				fixed distance = target->getLife().collisionDistance(checkPos);
				if (distance < radius)
				{
					collisionTargets[target->getPlayerId()] = target;
				}
			}
		}
	}

	if (context_->getOptionsGame().getActionSyncCheck() &&
		context_->getOptionsGame().getActionCollisionSyncCheck())
	{
		std::string targets;
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			++itor)
		{
			targets.append(S3D::formatStringBuffer("%u:%s ", 
				itor->second->getPlayerId(),
				itor->second->getLife().getTargetPosition().asQuickString()));
		}

		context_->getSimulator().addSyncCheck(
			S3D::formatStringBuffer("CollisionSet : %s %s \"%s\"", 
				position.asQuickString(),
				radius.asQuickString(),
				targets.c_str()));
	}
}

#ifndef S3D_SERVER
#include <GLEXT/GLState.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>

static void drawBox(Vector &position, Vector &size)
{
	float wid = size[0];
	float hgt = size[1];
	float dep = size[2];
	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glBegin(GL_LINE_LOOP);
			// Top
			glNormal3f(0,1,0);
			glVertex3f(-wid/2,hgt/2,dep/2);
			glVertex3f(wid/2,hgt/2,dep/2);
			glVertex3f(wid/2,hgt/2,-dep/2);
			glVertex3f(-wid/2,hgt/2,-dep/2);
		glEnd();
		glBegin(GL_LINE_LOOP);
			// Back
			glNormal3f(0,0,1);
			glVertex3f(-wid/2,hgt/2,dep/2);
			glVertex3f(-wid/2,-hgt/2,dep/2);
			glVertex3f(wid/2,-hgt/2,dep/2);
			glVertex3f(wid/2,hgt/2,dep/2);
		glEnd();
		glBegin(GL_LINE_LOOP);
			// Front
			glNormal3f(0,0,-1);
			glVertex3f(-wid/2,hgt/2,-dep/2);
			glVertex3f(wid/2,hgt/2,-dep/2);
			glVertex3f(wid/2,-hgt/2,-dep/2);
			glVertex3f(-wid/2,-hgt/2,-dep/2);
		glEnd();
		glBegin(GL_LINE_LOOP);
			// Left
			glNormal3f(1,0,0);
			glVertex3f(wid/2,hgt/2,-dep/2);
			glVertex3f(wid/2,hgt/2,dep/2);
			glVertex3f(wid/2,-hgt/2,dep/2);
			glVertex3f(wid/2,-hgt/2,-dep/2);
		glEnd();
		glBegin(GL_LINE_LOOP);
			// Right
			glNormal3f(-1,0,0);
			glVertex3f(-wid/2,hgt/2,dep/2);
			glVertex3f(-wid/2,hgt/2,-dep/2);
			glVertex3f(-wid/2,-hgt/2,-dep/2);
			glVertex3f(-wid/2,-hgt/2,dep/2);
		glEnd();
		glBegin(GL_LINE_LOOP);
			// Bottom
			glNormal3f(0,-1,0);
			glVertex3f(-wid/2,-hgt/2,dep/2);
			glVertex3f(-wid/2,-hgt/2,-dep/2);
			glVertex3f(wid/2,-hgt/2,-dep/2);
			glVertex3f(wid/2,-hgt/2,dep/2);
		glEnd();
	glPopMatrix();
}

void TargetSpace::draw()
{
	GLState glState(GLState::TEXTURE_OFF | GLState::LIGHTING_OFF);

	for (int b=0; b<spaceHSq_; b++)
	{
		for (int a=0; a<spaceWSq_; a++)
		{
			int num = spaceHSq_ * b + a;
			DIALOG_ASSERT(num >= 0 && num < noSquares_);

			Square *square = &squares_[num];
			if (square->targets.empty())
			{
				continue;
			}

			Vector position(a * spaceSq_ + spaceX_ + spaceSq_ / 2,
				b * spaceSq_ + spaceY_ + spaceSq_ / 2, 0);
			position[2] = 
				ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeight((int) position[0], (int) position[1]).asFloat() - 7.0f;
			Vector size(spaceSq_, spaceSq_, 20);

			if (position[0] > ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getLandscapeWidth() ||
				position[1] > ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getLandscapeHeight())
			{
				continue;
			}

			glColor3f(1.0f, 0.0f, 0.0f);
			drawBox(position, size);

			glBegin(GL_LINES);
			std::map<unsigned int, Target *>::iterator itor;
			for (itor = square->targets.begin();
				itor != square->targets.end();
				++itor)
			{
				Target *target = (*itor).second;
				glVertex3fv(target->getLife().getFloatPosition());
				glVertex3f(position[0], position[1], position[2] + 10.0f);
			}
			glEnd();
		}
	}
}
#endif
