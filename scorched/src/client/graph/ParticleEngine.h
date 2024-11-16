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

#if !defined(__INCLUDE_ParticleEngineh_INCLUDE__)
#define __INCLUDE_ParticleEngineh_INCLUDE__

#include <graph/Particle.h>
#include <engine/GameStateI.h>

class GLCamera;
class ParticleEngine : public GameStateI
{
public:
	ParticleEngine(GLCamera *camera, unsigned int maxParticles);
	virtual ~ParticleEngine();

	void setMaxParticles(unsigned int maxParticles);
	unsigned int getMaxParticles();
	unsigned int getParticlesOnScreen();
	GLCamera *getCamera() { return camera_; }
	void setAllowSorting(bool sorting) { allowSorting_ = sorting; }

	void killAll();

	Particle *getNextAliveParticle(unsigned int type);

	static void setFast(float speedMult) { speed_ = speedMult; }
	static float getFast() { return speed_; }

	// Inherited from GameStateI
	virtual void draw(const unsigned state);
	virtual void simulate(const unsigned int state, float simTime);

protected:
	Particle *particles_;
	Particle **usedParticles_;
	Particle **freeParticles_;
	GLCamera *camera_;

	float totalTime_;
	unsigned int maxParticles_;
	unsigned int particlesOnScreen_;
	bool allowSorting_;

	static float speed_;

	void normalizedSimulate(float time);

};

#endif // __INCLUDE_ParticleEngineh_INCLUDE__
