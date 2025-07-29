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

#if !defined(AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_)
#define AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_

#include <actions/Action.h>
#include <engine/PhysicsParticleObject.h>

class PhysicsParticle : 
	public Action,
	public PhysicsParticleObjectHandler
{
public:
	PhysicsParticle(bool referenced);
	virtual ~PhysicsParticle();

	virtual void collision(PhysicsParticleObject &position, 
		ScorchedCollisionId collisionId);

	PhysicsParticleActionObject &getPhysics() { return physicsObject_; }

	// Inherited from action
	virtual void simulate(fixed timepassed, bool &remove);

protected:
	PhysicsParticleActionObject physicsObject_;
	bool collision_;
	fixed totalActionTime_;

};

#endif // !defined(AFX_PhysicsParticle_H__53A44445_C8C9_4108_B5E1_744C0AF7A1D5__INCLUDED_)
