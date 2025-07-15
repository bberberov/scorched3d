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

#if !defined(__INCLUDE_DebrisActionRendererh_INCLUDE__)
#define __INCLUDE_DebrisActionRendererh_INCLUDE__

#include <common/Vector.h>
#include <graph/ModelRendererSimulator.h>
#include <graph/Particle.h>

class DebrisActionRenderer : public ParticleUserData
{
public:
	DebrisActionRenderer();
	virtual ~DebrisActionRenderer();

	void simulate(float time);
	void draw(Vector &position);

protected:
	ModelRendererSimulator *debris_;
	float rotationAng_;
	float rotationX_, rotationY_, rotationZ_;
	float rotationSpeed_;

};

#endif
