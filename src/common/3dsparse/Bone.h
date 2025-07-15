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

#if !defined(__INCLUDE_Boneh_INCLUDE__)
#define __INCLUDE_Boneh_INCLUDE__

#include <string>
#include <vector>
#include <common/FixedVector.h>

typedef fixed BoneMatrixType[3][4];
class BoneType
{
public:
	BoneType();

	BoneMatrixType relative_;
	BoneMatrixType absolute_;
	BoneMatrixType relativeFinal_;
	BoneMatrixType final_;
	int parent_;
};

class BonePositionKey
{
public:
	BonePositionKey(fixed time, FixedVector &pos);

	fixed time;
	FixedVector position;
};

class BoneRotationKey
{
public:
	BoneRotationKey(fixed time, FixedVector &rot);

	fixed time;
	FixedVector rotation;
};

class Bone
{
public:
	Bone(const char *name);
	virtual ~Bone();

	const char *getName() { return name_.c_str(); }
	const char *getParentName() { return parentName_.c_str(); }
	FixedVector &getPosition() { return position_; }
	FixedVector &getRotation() { return rotation_; }
	std::vector<BonePositionKey *> &getPositionKeys() 
		{ return  positionKeys_; }
	std::vector<BoneRotationKey *> &getRotationKeys()
		{ return rotationKeys_; }

	FixedVector &getPositionAtTime(fixed currentTime);
	void getRotationAtTime(fixed currentTime, BoneMatrixType &m);

	void addPositionKey(BonePositionKey *key) 
		{ positionKeys_.push_back(key); }
	void addRotationKey(BoneRotationKey *key) 
		{ rotationKeys_.push_back(key); }
	void setParentName(const char *parentName) 
		{ parentName_ = parentName; }
	void setPosition(FixedVector &pos)
		{ position_ = pos; }	
	void setRotation(FixedVector &rot)
		{ rotation_ = rot; }	

protected:
	std::string name_;
	std::string parentName_;
	FixedVector position_, rotation_;
	std::vector<BonePositionKey *> positionKeys_;
	std::vector<BoneRotationKey *> rotationKeys_;

};

#endif // __INCLUDE_Boneh_INCLUDE__
