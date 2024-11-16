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

#if !defined(__INCLUDE_VisibilityPatchQuadh_INCLUDE__)
#define __INCLUDE_VisibilityPatchQuadh_INCLUDE__

#include <land/VisibilityPatchInfo.h>
#include <common/Vector.h>

class VisibilityPatchGrid;
class VisibilityPatchQuad
{
public:
	VisibilityPatchQuad();
	~VisibilityPatchQuad();

	void setLocation(VisibilityPatchGrid *patchGrid, int x, int y, int size, 
		int mapwidth, int mapheight, int roofBaseHeight);
	void calculateGroundVisibility(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C);
	void calculateRoofVisibility(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C);

protected:
	int x_, y_;
	int size_;
	Vector position_, roofPosition_;

	WaterVisibilityPatch *waterVisibilityPatch_;
	LandVisibilityPatch *landVisibilityPatch_;
	RoofVisibilityPatch *roofVisibilityPatch_;
	TargetVisibilityPatch *targetVisibilityPatch_;
	VisibilityPatchQuad *topLeft_, *topRight_;
	VisibilityPatchQuad *botLeft_, *botRight_;

	void setGroundNotVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos);
	void setGroundVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C);
	void setRoofNotVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos);
	void setRoofVisible(VisibilityPatchInfo &patchInfo, Vector &cameraPos, float C);
};

#endif // __INCLUDE_VisibilityPatchQuadh_INCLUDE__
