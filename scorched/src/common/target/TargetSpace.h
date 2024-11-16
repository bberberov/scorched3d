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

#if !defined(__INCLUDE_TargetSpaceh_INCLUDE__)
#define __INCLUDE_TargetSpaceh_INCLUDE__

#include <target/Target.h>
#include <map>
#include <set>
#include <vector>

class ScorchedContext;
class TargetSpace
{
public:
	TargetSpace();
	virtual ~TargetSpace();

	void clear();

	void updateTarget(Target *target);
	void setContext(ScorchedContext *context) { context_ = context; }

	Target *getCollision(FixedVector &position);
	void getCollisionSet(FixedVector &position, fixed radius, 
		std::map<unsigned int, Target *> &collisionTargets, 
		bool ignoreHeight = false);

	void draw();

protected:
	class Square
	{
	public:
		std::map<unsigned int, Target *> targets;
		int squarenum;
	};

	ScorchedContext *context_;
	Square *squares_;
	int spaceX_, spaceY_; // Position of bottom left of space
	int spaceW_, spaceH_; // Width, height of space
	int spaceWSq_, spaceHSq_; // Number of squares in width and height
	int spaceSq_; // Width of each square
	int noSquares_; // The total number of squares

	void normalizeCoords(int &x, int &y)
	{
		// Make sure x and y are in the space
		x = MIN(x, spaceW_ + spaceX_ - 1);
		x = MAX(x, spaceX_);
		y = MIN(y, spaceH_ + spaceY_ - 1);
		y = MAX(y, spaceY_);

		// Find the square pos
		x -= spaceX_;
		y -= spaceY_;
		x /= spaceSq_;
		y /= spaceSq_;
	}

	void getSquares(Target *target, std::vector<Square*> &squares);
	void removeTarget(Target *target);

private:
	TargetSpace(TargetSpace &other);
	TargetSpace &operator=(TargetSpace &other);
};

#endif // __INCLUDE_TargetSpaceh_INCLUDE__
