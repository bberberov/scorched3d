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

#if !defined(__INCLUDE_WaterVisibilityPatchh_INCLUDE__)
#define __INCLUDE_WaterVisibilityPatchh_INCLUDE__

#include <common/Vector.h>

class MipMapPatchIndexs;
class MipMapPatchIndex;

class WaterVisibilityPatch
{
public:
	WaterVisibilityPatch();
	~WaterVisibilityPatch();

	void setLocation(int x, int y,
		int patchX, int patchY,
		WaterVisibilityPatch *leftPatch, 
		WaterVisibilityPatch *rightPatch, 
		WaterVisibilityPatch *topPatch, 
		WaterVisibilityPatch *bottomPatch);
	bool setVisible(Vector &cameraPos);
	void setNotVisible();

	int getVisibilityIndex() { return visible_?visibilityIndex_:-1; }
	int getPatchX() { return patchX_; }
	int getPatchY() { return patchY_; }
	int getPatchIndex() { return patchIndex_; }
	Vector &getOffset() { return offset_; }
	Vector &getPosition() { return position_; }

	WaterVisibilityPatch *getLeftPatch() { return leftPatch_; }
	WaterVisibilityPatch *getRightPatch() { return rightPatch_; }
	WaterVisibilityPatch *getTopPatch() { return topPatch_; }
	WaterVisibilityPatch *getBottomPatch() { return bottomPatch_; }

protected:
	int x_, y_;
	int patchX_, patchY_, patchIndex_;
	int visibilityIndex_;
	bool visible_;
	Vector offset_, position_;
	WaterVisibilityPatch *leftPatch_;
	WaterVisibilityPatch *rightPatch_; 
	WaterVisibilityPatch *topPatch_;
	WaterVisibilityPatch *bottomPatch_;
};

#endif // __INCLUDE_WaterVisibilityPatchh_INCLUDE__
