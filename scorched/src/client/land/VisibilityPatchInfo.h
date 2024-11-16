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

#if !defined(__INCLUDE_VisibilityPatchInfoh_INCLUDE__)
#define __INCLUDE_VisibilityPatchInfoh_INCLUDE__

#include <land/LandVisibilityPatch.h>
#include <land/WaterVisibilityPatch.h>
#include <land/RoofVisibilityPatch.h>
#include <land/TargetVisibilityPatch.h>
#include <land/TargetList.h>

class VisibilityPatchInfo
{
public:
	VisibilityPatchInfo();
	~VisibilityPatchInfo();

	unsigned int &getPatchesVisitedCount() {
		return patchesVisited_; }

	int getVisibleLandPatchesCount() { 
		return landVisibility_.getObjectCount(); }
	int getVisibleRoofPatchesCount() { 
		return roofVisibility_.getObjectCount(); }
	int getVisibleWaterPatchesCount() { 
		return waterVisibility_[0].getObjectCount() +
			waterVisibility_[1].getObjectCount() +
			waterVisibility_[2].getObjectCount() +
			waterVisibility_[3].getObjectCount(); }

	void reset();
	void generate(int maxLandPatches, int maxWaterPatches, int maxTargetPatches);

	TargetList &getLandVisibility() 
	{
		return landVisibility_;
	}

	TargetList &getRoofVisibility() 
	{
		return roofVisibility_;
	}

	TargetList &getWaterVisibility(int index) 
	{
		DIALOG_ASSERT(index >= 0 && index <= 4);
		return waterVisibility_[index];
	}

	TargetList &getTreeVisibility()
	{
		return treeVisibility_;
	}

	TargetList &getTargetVisibility()
	{
		return targetVisibility_;
	}

protected:
	unsigned int patchesVisited_;
	TargetList waterVisibility_[4];
	TargetList landVisibility_;
	TargetList roofVisibility_;
	TargetList treeVisibility_;
	TargetList targetVisibility_;

	void clear();
};

#endif // __INCLUDE_VisibilityPatchInfoh_INCLUDE__
