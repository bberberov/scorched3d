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

#if !defined(__INCLUDE_Modelh_INCLUDE__)
#define __INCLUDE_Modelh_INCLUDE__

#include <3dsparse/Mesh.h>
#include <3dsparse/Bone.h>

class Model
{
public:
	Model();
	virtual ~Model();

	FixedVector &getMin() { return min_; }
	FixedVector &getMax() { return max_; }
	std::vector<Mesh *> &getMeshes() { return meshes_; }
	std::vector<BoneType *> &getBaseBoneTypes() { return baseBoneTypes_; }
	std::vector<Bone *> &getBones() { return bones_; }
	int getNumberTriangles() { return noTriangles_; }
	int getStartFrame() { return startFrame_; }
	int getTotalFrames() { return totalFrames_; }
	bool getTexturesUsed() { return texturesUsed_; }

	// Used during building
	void setup();
	void addMesh(Mesh *mesh) { meshes_.push_back(mesh); }
	void addBone(Bone *bone) { bones_.push_back(bone); }
	void setStartFrame(int frame) { startFrame_ = frame; }
	void setTotalFrames(int frames) { totalFrames_ = frames; }

protected:
	bool texturesUsed_;
	std::vector<Mesh *> meshes_;
	std::vector<Bone *> bones_;
	std::vector<BoneType *> baseBoneTypes_;
	FixedVector min_, max_;

	int noTriangles_;
	int startFrame_;
	int totalFrames_;

	void centre();
	void countTextures();
	void setupBones();
	void setupColor();
};

#endif // __INCLUDE_Modelh_INCLUDE__
