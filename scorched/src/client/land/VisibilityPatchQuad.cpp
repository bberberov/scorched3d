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

#include <land/VisibilityPatchQuad.h>
#include <land/VisibilityPatchGrid.h>
#include <GLEXT/GLCameraFrustum.h>

VisibilityPatchQuad::VisibilityPatchQuad() :
	topLeft_(0), topRight_(0),
	botLeft_(0), botRight_(0),
	landVisibilityPatch_(0),
	roofVisibilityPatch_(0),
	waterVisibilityPatch_(0),
	targetVisibilityPatch_(0)
{
}

VisibilityPatchQuad::~VisibilityPatchQuad()
{
	delete topLeft_; delete topRight_;
	delete botLeft_; delete botRight_;
}

void VisibilityPatchQuad::setLocation(VisibilityPatchGrid *patchGrid, int x, int y, int size,
	int mapwidth, int mapheight, int roofBaseHeight)
{
	x_ = x; y_ = y; size_ = size;
	position_ = Vector(x_ + size_ / 2, y_ + size_ / 2);
	roofPosition_ = Vector(x_ + size_ / 2, y_ + size_ / 2, roofBaseHeight);

	int endSize = 512;
	if ((x + size_) > 0 && 
		(y + size_) > 0 &&
		x < mapwidth && y < mapheight) 
	{
		endSize = 32;
		if (size == 32)
		{
			// Land, Roof, Target
			landVisibilityPatch_ = patchGrid->getLandVisibilityPatch(x, y);
			roofVisibilityPatch_ = patchGrid->getRoofVisibilityPatch(x, y);
			targetVisibilityPatch_ = patchGrid->getTargetVisibilityPatch(x, y);
		}
		if (size == 128)
		{
			// Water
			waterVisibilityPatch_ = patchGrid->getWaterVisibilityPatch(x, y);
		}
	}
	else 
	{
		endSize = 128;
		if (size == 128)
		{
			// Water, Target
			waterVisibilityPatch_ = patchGrid->getWaterVisibilityPatch(x, y);
			targetVisibilityPatch_ = patchGrid->getTargetVisibilityPatch(x, y);
		}
	}

	if (size > endSize)
	{
		topLeft_ = new VisibilityPatchQuad();
		topRight_ = new VisibilityPatchQuad();
		botLeft_ = new VisibilityPatchQuad();
		botRight_ = new VisibilityPatchQuad();

		topLeft_->setLocation(patchGrid, x, y, size / 2, mapwidth, mapheight, roofBaseHeight);
		topRight_->setLocation(patchGrid, x + size / 2, y, size / 2, mapwidth, mapheight, roofBaseHeight);
		botLeft_->setLocation(patchGrid, x, y + size / 2, size / 2, mapwidth, mapheight, roofBaseHeight);
		botRight_->setLocation(patchGrid, x + size / 2, y + size / 2, size / 2, mapwidth, mapheight, roofBaseHeight);
	}
}

void VisibilityPatchQuad::setGroundNotVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos)
{
	patchInfo.getPatchesVisitedCount()++;

	if (landVisibilityPatch_) landVisibilityPatch_->setNotVisible();
	if (targetVisibilityPatch_) targetVisibilityPatch_->setNotVisible();
	if (waterVisibilityPatch_) waterVisibilityPatch_->setNotVisible();

	// Update Children
	if (topLeft_) topLeft_->setGroundNotVisible(patchInfo, cameraPos);
	if (topRight_) topRight_->setGroundNotVisible(patchInfo, cameraPos);
	if (botLeft_) botLeft_->setGroundNotVisible(patchInfo, cameraPos);
	if (botRight_) botRight_->setGroundNotVisible(patchInfo, cameraPos);
}

void VisibilityPatchQuad::setGroundVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C)
{
	patchInfo.getPatchesVisitedCount()++;

	if (landVisibilityPatch_)
	{
		float distance = (cameraPos - landVisibilityPatch_->getPosition()).Magnitude();
		if (landVisibilityPatch_->setVisible(distance, C))
		{
			patchInfo.getLandVisibility().add(landVisibilityPatch_);
		}
	}
	if (targetVisibilityPatch_)
	{
		if (targetVisibilityPatch_->hasTargets() ||
			targetVisibilityPatch_->hasTrees())
		{
			float distance = (cameraPos - targetVisibilityPatch_->getPosition()).Magnitude();
			if (targetVisibilityPatch_->setVisible(distance))
			{
				if (targetVisibilityPatch_->hasTargets()) 
				{
					patchInfo.getTargetVisibility().add(targetVisibilityPatch_);
				}
				if (targetVisibilityPatch_->hasTrees()) 
				{
					patchInfo.getTreeVisibility().add(targetVisibilityPatch_);
				}
			}
		}
	}

	if (waterVisibilityPatch_)
	{
		if (waterVisibilityPatch_->setVisible(cameraPos))
		{
			patchInfo.getWaterVisibility(waterVisibilityPatch_->getPatchIndex()).add(waterVisibilityPatch_);
		}
	}

	// Update Children
	if (topLeft_) topLeft_->calculateGroundVisibility(patchInfo, cameraPos, C);
	if (topRight_) topRight_->calculateGroundVisibility(patchInfo, cameraPos, C);
	if (botLeft_) botLeft_->calculateGroundVisibility(patchInfo, cameraPos, C);
	if (botRight_) botRight_->calculateGroundVisibility(patchInfo, cameraPos, C);	
}

void VisibilityPatchQuad::calculateGroundVisibility(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C)
{
	Vector *position = &position_;
	float size = float(size_);
	if (landVisibilityPatch_)
	{
		position = &landVisibilityPatch_->getPosition();
		size = landVisibilityPatch_->getBoundingSize();
	}

	bool visible = true;
	if (size_ == 32 || size_ == 128 || size_ == 512)
	{
		visible = GLCameraFrustum::instance()->sphereInFrustum(*position, size);
	}

	if (!visible)
	{
		setGroundNotVisible(patchInfo, cameraPos);
	}
	else
	{
		setGroundVisible(patchInfo, cameraPos, C);
	}
}


void VisibilityPatchQuad::setRoofNotVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos)
{
	patchInfo.getPatchesVisitedCount()++;

	if (roofVisibilityPatch_) roofVisibilityPatch_->setNotVisible();

	// Update Children
	if (topLeft_) topLeft_->setRoofNotVisible(patchInfo, cameraPos);
	if (topRight_) topRight_->setRoofNotVisible(patchInfo, cameraPos);
	if (botLeft_) botLeft_->setRoofNotVisible(patchInfo, cameraPos);
	if (botRight_) botRight_->setRoofNotVisible(patchInfo, cameraPos);
}

void VisibilityPatchQuad::setRoofVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C)
{
	patchInfo.getPatchesVisitedCount()++;

	if (roofVisibilityPatch_)
	{
		float distance = (cameraPos - roofVisibilityPatch_->getPosition()).Magnitude();
		if (roofVisibilityPatch_->setVisible(distance, C))
		{
			patchInfo.getRoofVisibility().add(roofVisibilityPatch_);
		}
	}

	// Update Children
	if (topLeft_) topLeft_->calculateRoofVisibility(patchInfo, cameraPos, C);
	if (topRight_) topRight_->calculateRoofVisibility(patchInfo, cameraPos, C);
	if (botLeft_) botLeft_->calculateRoofVisibility(patchInfo, cameraPos, C);
	if (botRight_) botRight_->calculateRoofVisibility(patchInfo, cameraPos, C);	
}

void VisibilityPatchQuad::calculateRoofVisibility(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C)
{
	Vector *position = &roofPosition_;
	float size = float(size_);
	if (roofVisibilityPatch_)
	{
		position = &roofVisibilityPatch_->getPosition();
		size = roofVisibilityPatch_->getBoundingSize();
	}

	bool visible = true;
	if (size_ == 32 || size_ == 128 || size_ == 512)
	{
		visible = GLCameraFrustum::instance()->sphereInFrustum(*position, size);
	}

	if (!visible)
	{
		setRoofNotVisible(patchInfo, cameraPos);
	}
	else
	{
		setRoofVisible(patchInfo, cameraPos, C);
	}
}
