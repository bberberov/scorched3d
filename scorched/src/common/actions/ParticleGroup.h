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

#if !defined(__INCLUDE_ParticleGrouph_INCLUDE__)
#define __INCLUDE_ParticleGrouph_INCLUDE__

#include <engine/ObjectGroupEntry.h>

class PhysicsParticle;
class WeaponFireContext;
class ScorchedContext;
class ParticleGroup : public ObjectGroupEntry
{
public:
	ParticleGroup(ScorchedContext &context, 
		PhysicsParticle *physicsParticle, WeaponFireContext *weaponFireContext);
	virtual ~ParticleGroup();

	virtual void *getObject();
	virtual ObjectGroupEntry::ObjectType getType();
	virtual FixedVector &getPosition();
	virtual FixedVector &getVelocity();
	virtual unsigned int getPlayerId();

protected:
	PhysicsParticle *physicsParticle_;
	WeaponFireContext *weaponFireContext_;
};

#endif // __INCLUDE_ParticleGrouph_INCLUDE__
