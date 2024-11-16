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

#if !defined(__INCLUDE_ExplosionNukeRendererh_INCLUDE__)
#define __INCLUDE_ExplosionNukeRendererh_INCLUDE__

#include <actions/Action.h>
#include <graph/ParticleEmitter.h>
#include <common/Vector.h>
#include <list>

#define ExplosionNukeRenderer_STEPS 100

class ExplosionNukeRendererEntry : public ParticleUserData
{
public:
	ExplosionNukeRendererEntry(Vector &position, float size);
	virtual ~ExplosionNukeRendererEntry();

	void simulate(Particle *particle, float time);

	float getCloudRotation() { return cloudRotation_; }
	Vector &getRotation() { return rotation_; }

protected:
	float size_;
	float totalTime_;
	float cloudRotation_;
	Vector rotation_;
	Vector startPosition_;
};

class ExplosionNukeRenderer : public ActionRenderer
{
public:
	ExplosionNukeRenderer(Vector &position, float size, GLTextureSet *set, bool animate);
	virtual ~ExplosionNukeRenderer();

	virtual void draw(Action *action);
	virtual void simulate(Action *action, float frameTime, bool &remove);

	static Vector *positions_;
protected:
	GLTextureSet *set_;
	bool animate_;
	Vector position_;
	float size_;
	float time_;
	float totalTime_;
	ParticleEmitter emitter_;
};

#endif
