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

#if !defined(__INCLUDE_HeightMapVisibilityPatchh_INCLUDE__)
#define __INCLUDE_HeightMapVisibilityPatchh_INCLUDE__

#include <common/Vector.h>

class HeightMap;
class MipMapPatchIndex;

class HeightMapVisibilityPatch
{
public:
	HeightMapVisibilityPatch(HeightMap *heightMap);
	virtual ~HeightMapVisibilityPatch();

	void setLocation(int x, int y,
		HeightMapVisibilityPatch *leftPatch, 
		HeightMapVisibilityPatch *rightPatch, 
		HeightMapVisibilityPatch *topPatch, 
		HeightMapVisibilityPatch *bottomPatch);
	bool setVisible(float distance, float C);
	void setNotVisible();
	void setRecalculateErrors() { recalculateErrors_ = true; }

	int getVisibilityIndex() { return visible_?visibilityIndex_:-1; }
	Vector &getPosition() { return position_; }
	float getBoundingSize() { return boundingSize_; }

	HeightMapVisibilityPatch *getLeftPatch() { return leftPatch_; }
	HeightMapVisibilityPatch *getRightPatch() { return rightPatch_; }
	HeightMapVisibilityPatch *getTopPatch() { return topPatch_; }
	HeightMapVisibilityPatch *getBottomPatch() { return bottomPatch_; }

	void draw(MipMapPatchIndex &index, bool simple);
	void drawLODLevel(MipMapPatchIndex &index);

protected:
	int x_, y_;
	int dataOffSet_;
	float maxHeight_, minHeight_, boundingSize_;
	int dataSize_;
	int visibilityIndex_;
	bool visible_, recalculateErrors_;
	float indexErrors_[5];
	Vector position_;
	HeightMap *heightMap_;
	HeightMapVisibilityPatch *leftPatch_;
	HeightMapVisibilityPatch *rightPatch_; 
	HeightMapVisibilityPatch *topPatch_;
	HeightMapVisibilityPatch *bottomPatch_;

	virtual void calculateErrors();
	float getHeight(int x, int y);
	float calculateError(int x1, int x2, int y1, int y2,
		float x1y1, float x2y2, float x1y2, float x2y1);
	float calculateError2(int x, int y, int width, 
		float &minHeight, float &maxHeight);
};

#endif // __INCLUDE_HeightMapVisibilityPatchh_INCLUDE__
