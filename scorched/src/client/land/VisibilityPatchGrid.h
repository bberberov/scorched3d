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

#if !defined(__INCLUDE_VisibilityPatchGridh_INCLUDE__)
#define __INCLUDE_VisibilityPatchGridh_INCLUDE__

#include <land/VisibilityPatchQuad.h>
#include <land/VisibilityPatchInfo.h>
#include <land/LandSurround.h>
#include <geomipmap/MipMapPatchIndexs.h>

class Target;
class GLSLShaderSetup;
class WaterVisibilityPatch;
class LandAndTargetVisibilityPatch;
class WaterAndTargetVisibilityPatch;
class Water2Patches;
class GraphicalLandscapeMap;
class VisibilityPatchGrid
{
public:
	static VisibilityPatchGrid *instance();

	void generate();

	void calculateVisibility();
	void recalculateLandscapeErrors(FixedVector &position, fixed size);
	void recalculateRoofErrors(FixedVector &position, fixed size);

	void drawLand(int addIndex, bool verticesOnly, bool allPatches);
	void drawRoof(int addIndex, bool verticesOnly, bool allPatches);
	void drawLandLODLevels();
	void drawSurround();
	void drawWater(Water2Patches &patches, 
		MipMapPatchIndexs &indexes, Vector &cameraPosition, 
		Vector landscapeSize,
		GLSLShaderSetup *waterShader);

	int getEpocNumber() { return epoc_; }
	int getVisibleLandPatchesCount() { 
		return patchInfo_.getVisibleLandPatchesCount(); }
	int getVisibleWaterPatchesCount() {
		return patchInfo_.getVisibleWaterPatchesCount(); }
	int getVisibleRoofPatchesCount() {
		return patchInfo_.getVisibleRoofPatchesCount(); }
	int getPatchesVisitedCount() {
		return patchInfo_.getPatchesVisitedCount(); }

	VisibilityPatchInfo &getPatchInfo() { return patchInfo_; }

	LandVisibilityPatch *getLandVisibilityPatch(int x, int y);
	RoofVisibilityPatch *getRoofVisibilityPatch(int x, int y);
	TargetVisibilityPatch *getTargetVisibilityPatch(int x, int y);
	WaterVisibilityPatch *getWaterVisibilityPatch(int x, int y);
protected:
	int epoc_;
	LandSurround surround_;
	MipMapPatchIndexs landIndexs_;

	// All the visibility patches
	VisibilityPatchInfo patchInfo_;
	LandAndTargetVisibilityPatch *landPatches_;
	WaterAndTargetVisibilityPatch *waterPatches_;

	// The visibility data that decides if a visibility patch is visible or not
	VisibilityPatchQuad *visibilityPatches_;

	// The size of the patches
	int midX_, midY_;
	int landWidth_, landHeight_;
	int waterWidth_, waterHeight_;
	int visibilityWidth_, visibilityHeight_;

	void clear();
	void drawHeightMap(GraphicalLandscapeMap *landscapeMap, int addIndex, bool verticesOnly, bool allPatches, bool roof);
	void recalculateErrors(FixedVector &position, fixed size, bool roof);

private:
	VisibilityPatchGrid();
	~VisibilityPatchGrid();
};

#endif // __INCLUDE_VisibilityPatchGridh_INCLUDE__
