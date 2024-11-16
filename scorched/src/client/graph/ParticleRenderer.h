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

#if !defined(__INCLUDE_ParticleRendererh_INCLUDE__)
#define __INCLUDE_ParticleRendererh_INCLUDE__

class Particle;
class ParticleRenderer
{
public:
	virtual void renderParticle(Particle &particle) = 0;
	virtual void simulateParticle(Particle &particle, float time) = 0;
	virtual void recycleParticle(Particle &particle) { }
};

class ParticleRendererPoints : public ParticleRenderer
{
public:
	static ParticleRendererPoints *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererQuads : public ParticleRenderer
{
public:
	static ParticleRendererQuads *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererDebris : public ParticleRenderer
{
public:
	static ParticleRendererDebris *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererSmoke : public ParticleRenderer
{
public:
	static ParticleRendererSmoke *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererNapalm : public ParticleRenderer
{
public:
	static ParticleRendererNapalm *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererMushroom : public ParticleRenderer
{
public:
	static ParticleRendererMushroom *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererRain : public ParticleRenderer
{
public:
	static ParticleRendererRain *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererSnow : public ParticleRenderer
{
public:
	static ParticleRendererSnow *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

class ParticleRendererWall : public ParticleRenderer
{
public:
	static ParticleRendererWall *getInstance();
	virtual void renderParticle(Particle &particle);
	virtual void simulateParticle(Particle &particle, float time);
};

#endif // __INCLUDE_ParticleRendererh_INCLUDE__
