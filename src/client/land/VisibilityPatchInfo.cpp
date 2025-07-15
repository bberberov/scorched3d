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

#include <land/VisibilityPatchInfo.h>

VisibilityPatchInfo::VisibilityPatchInfo()
{
}

VisibilityPatchInfo::~VisibilityPatchInfo()
{
}

void VisibilityPatchInfo::clear()
{
	landVisibility_.clear();
	roofVisibility_.clear();
	treeVisibility_.clear();
	targetVisibility_.clear();
	for (int i=0; i<4; i++) waterVisibility_[i].clear();
}

void VisibilityPatchInfo::generate(int maxLandPatches, int maxWaterPatches, int maxTargetPatches)
{
	clear();

	landVisibility_.setCapacity(maxLandPatches);
	roofVisibility_.setCapacity(maxLandPatches);
	treeVisibility_.setCapacity(maxTargetPatches);
	targetVisibility_.setCapacity(maxTargetPatches);
	for (int i=0; i<4; i++) waterVisibility_[i].setCapacity(maxWaterPatches);

	reset();
}

void VisibilityPatchInfo::reset()
{
	patchesVisited_ = 0;
	landVisibility_.reset();
	roofVisibility_.reset();
	treeVisibility_.reset();
	targetVisibility_.reset();
	for (int i=0; i<4; i++) waterVisibility_[i].reset();
}
