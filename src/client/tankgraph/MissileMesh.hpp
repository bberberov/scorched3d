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

#ifndef __INCLUDE_MissileMesh_hpp_INCLUDE__
#define __INCLUDE_MissileMesh_hpp_INCLUDE__

#include <common/Vector.hpp>
#include <list>

class ModelID;
class ModelRenderer;

class MissileMesh
{
public:
	MissileMesh( ModelID& missile );
	virtual ~MissileMesh();

	void draw( Vector& position, Vector& direction, int flareType, float rotation, Vector& rotationAxis, float frame );
	void setScale( float scale );

protected:
	float          innerScale_;
	float          scale_;
	ModelRenderer* model_;

	struct FlareInfo
	{
		Vector position;
		float  size;
	};

	std::list< FlareInfo > flares_;
};

#endif  // __INCLUDE_MissileMesh_hpp_INCLUDE__
