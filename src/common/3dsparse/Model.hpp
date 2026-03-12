////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_Model_hpp_INCLUDE__
#define __INCLUDE_Model_hpp_INCLUDE__

#include <3dsparse/Mesh.hpp>
#include <3dsparse/Bone.hpp>

class Model
{
public:
	Model();
	virtual ~Model();

	// clang-format off
	// uncrustify off
	FixedVector&              getMin()             { return min_; }
	FixedVector&              getMax()             { return max_; }
	std::vector< Mesh* >&     getMeshes()          { return meshes_; }
	std::vector< BoneType* >& getBaseBoneTypes()   { return baseBoneTypes_; }
	std::vector< Bone* >&     getBones()           { return bones_; }
	int                       getNumberTriangles() { return noTriangles_; }
	int                       getStartFrame()      { return startFrame_; }
	int                       getTotalFrames()     { return totalFrames_; }
	bool                      getTexturesUsed()    { return texturesUsed_; }

	// Used during building
	void setup();
	void addMesh( Mesh* mesh )        { meshes_.push_back( mesh ); }
	void addBone( Bone* bone )        { bones_.push_back( bone ); }
	void setStartFrame( int frame )   { startFrame_ = frame; }
	void setTotalFrames( int frames ) { totalFrames_ = frames; }
	// uncrustify on
	// clang-format on

protected:
	std::vector< Mesh* >     meshes_;
	std::vector< Bone* >     bones_;
	std::vector< BoneType* > baseBoneTypes_;
	FixedVector              min_, max_;

	int  startFrame_;
	int  totalFrames_;
	int  noTriangles_;
	bool texturesUsed_;

	void centre();
	void countTextures();
	void setupBones();
	void setupColor();
};

#endif  // __INCLUDE_Model_hpp_INCLUDE__
