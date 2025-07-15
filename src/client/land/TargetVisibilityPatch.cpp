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

#include <land/TargetVisibilityPatch.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <graph/OptionsDisplay.h>

std::set<void *> TargetVisibilityPatch::largeTargets_;

TargetVisibilityPatch::TargetVisibilityPatch() : 
	visible_(false), distance_(0.0f)
{
}

TargetVisibilityPatch::~TargetVisibilityPatch()
{
}

void TargetVisibilityPatch::setLocation(int x, int y, int w, int h)
{
	// Set location and neighbors
	x_ = x; y_ = y;
	float heightRange = 0.0f;
	float minHeight = 0.0f;
	position_ = Vector(float(x_ + w / 2), float(y_ + h / 2), 
		heightRange / 2.0f + minHeight);
}

bool TargetVisibilityPatch::setVisible(float distance)
{ 
	//if (distance < OptionsDisplay::instance()->getDrawCullingDistance() + 100.0f)
	{
		distance_ = distance;
		visible_ = true;
	}
	//else
	{
		//visible_ = false;
	}
	return visible_;
}

void TargetVisibilityPatch::setNotVisible()
{
	visible_ = false;
}
