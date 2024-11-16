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

#include <land/WaterVisibilityPatch.h>
#include <land/VisibilityPatchGrid.h>
#include <landscape/Landscape.h>
#include <water/Water.h>
#include <geomipmap/MipMapPatchIndexs.h>
#include <graph/OptionsDisplay.h>

WaterVisibilityPatch::WaterVisibilityPatch() : 
	visible_(false),
	leftPatch_(0), rightPatch_(0),
	topPatch_(0), bottomPatch_(0),
	visibilityIndex_(4)
{
}

WaterVisibilityPatch::~WaterVisibilityPatch()
{
}

void WaterVisibilityPatch::setLocation(int x, int y,
	int patchX, int patchY,
	WaterVisibilityPatch *leftPatch, 
	WaterVisibilityPatch *rightPatch, 
	WaterVisibilityPatch *topPatch, 
	WaterVisibilityPatch *bottomPatch)
{
	x_ = x; y_ = y;
	leftPatch_ = leftPatch;
	rightPatch_ = rightPatch;
	topPatch_ = topPatch;
	bottomPatch_ = bottomPatch;

	patchX_ = patchX;
	patchY_ = patchY;
	patchIndex_ = patchX_ + patchY_ * 2;

	offset_ = Vector(x_, y_, 0);
	position_ = Vector(x_ + 64, y_ + 64, 5);
}

bool WaterVisibilityPatch::setVisible(Vector &cameraPos)
{ 
	float distance = (cameraPos - position_).Magnitude();
	visibilityIndex_ = 6;
	if (!OptionsDisplay::instance()->getNoWaterLOD())
	{
		float *waterIndexErrors = Landscape::instance()->getWater().getIndexErrors();
		int waterDetailLevelRamp = 
			OptionsDisplay::instance()->getWaterDetailLevelRamp();

		visibilityIndex_ = int(distance - 50.0f) / waterDetailLevelRamp;
		if (waterIndexErrors[6] < 1.0f) visibilityIndex_ += 3;
		else if (waterIndexErrors[6] < 2.0f) visibilityIndex_ += 2;
		else if (waterIndexErrors[6] < 3.0f) visibilityIndex_ += 1;

		if (OptionsDisplay::instance()->getNoWaterMovement())
		{
			visibilityIndex_ += 3;
		}
		visibilityIndex_ = MAX(0, MIN(visibilityIndex_, 6));
	}

	if (distance < 2000.0f) 
	{
		visible_ = true;
		return true;
	}

	visible_ = false;
	return false;
}

void WaterVisibilityPatch::setNotVisible()
{
	visible_ = false;
}
