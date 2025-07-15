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

#include <common/Defines.h>
#include <common/VectorLib.h>
#include <graph/ParticleEmitter.h>
#include <graph/ParticleTypes.h>
#include <sprites/DebrisActionRenderer.h>
#include <sprites/SmokeActionRenderer.h>
#include <sprites/ExplosionTextures.h>
#include <sprites/NapalmRenderer.h>
#include <sprites/ExplosionNukeRenderer.h>
#include <sprites/WallActionRenderer.h>
#include <landscape/Landscape.h>
#include <math.h>
#include <stdlib.h>

static void randomVector(Vector &result, Vector &start, Vector &end)
{
	result[0] = start[0] + ((end[0] - start[0]) * RAND);
	result[1] = start[1] + ((end[1] - start[1]) * RAND);
	result[2] = start[2] + ((end[2] - start[2]) * RAND);
}

static void randomCounterVector(Vector &result, Vector &current, 
								Vector &start, Vector &end,
								float life)
{
	randomVector(result, start, end);
	result -= current;
	result /= life;
}

static float randomScalar(float start, float end)
{
	return start + ((end - start) * RAND);
}

static float randomCounterScalar(float current, 
								float start, float end,
								float life)
{
	float result = randomScalar(start, end);
	result -= current;
	result /= life;
	return result;
}

ParticleEmitter::ParticleEmitter()
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::setLife(float life1, float life2)
{
	life1_ = life1; life2_ = life2;
}

void ParticleEmitter::setMass(float mass1, float mass2)
{
	mass1_ = mass1; mass2_ = mass2;
}

void ParticleEmitter::setFriction(float friction1, float friction2)
{
	friction1_ = friction1; friction2_ = friction2;
}
	
void ParticleEmitter::setStartColor(
		Vector startColor1, float startAlpha1,
		Vector startColor2, float startAlpha2)
{
	startAlpha1_ = startAlpha1; endAlpha1_ = startAlpha1; 
	startAlpha2_ = startAlpha2; endAlpha2_ = startAlpha2;
	startColor1_ = startColor1; endColor1_ = startColor1;
	startColor2_ = startColor2; endColor2_ = startColor2;
}

void ParticleEmitter::setEndColor(
		Vector endColor1, float endAlpha1,
		Vector endColor2, float endAlpha2)
{
	endColor1_ = endColor1; endAlpha1_ = endAlpha1;
	endColor2_ = endColor2; endAlpha2_ = endAlpha2;
}

void ParticleEmitter::setVelocity(Vector velocity1, Vector velocity2)
{
	velocity1_ = velocity1; velocity2_ = velocity2;
}

void ParticleEmitter::setStartSize(
		float startX1, float startY1,
		float startX2, float startY2)
{
	startSize1_[0] = startX1; startSize1_[1] = startY1;
	startSize2_[0] = startX2; startSize2_[1] = startY2;
}

void ParticleEmitter::setEndSize(
		float endX1, float endY1,
		float endX2, float endY2)
{
	endSize1_[0] = endX1; endSize1_[1] = endY1;
	endSize2_[0] = endX2; endSize2_[1] = endY2;
}

void ParticleEmitter::setGravity(Vector gravity)
{
	gravity_ = gravity;
}

void ParticleEmitter::setAdditiveTexture(bool additiveTexture)
{
	additiveTexture_ = additiveTexture;
}

void ParticleEmitter::setWindAffect(bool windAffect)
{
	windAffect_ = windAffect;
}

void ParticleEmitter::setAttributes(
	float life1, float life2,
	float mass1, float mass2,
	float friction1, float friction2,
	Vector velocity1, Vector velocity2,
	Vector startColor1, float startAlpha1,
	Vector startColor2, float startAlpha2,
	Vector endColor1, float endAlpha1,
	Vector endColor2, float endAlpha2,
	float startX1, float startY1,
	float startX2, float startY2,
	float endX1, float endY1,
	float endX2, float endY2,
	Vector gravity,
	bool additiveTexture,
	bool windAffect)
{
	setLife(life1, life2);
	setMass(mass1, mass2);
	setFriction(friction1, friction2);
	setVelocity(velocity1, velocity2);
	setStartColor(startColor1, startAlpha1,
		startColor2, startAlpha2);
	setEndColor(endColor1, endAlpha1,
		endColor2, endAlpha2);
	setStartSize(startX1, startY1,
		startX2, startY2);
	setEndSize(endX1, endY1,
		endX2, endY2);
	setGravity(gravity);
	setAdditiveTexture(additiveTexture);
	setWindAffect(windAffect);
}

void ParticleEmitter::createDefaultParticle(Particle &particle)
{
	float life = randomScalar(life1_, life2_);
	float mass = randomScalar(mass1_, mass2_);
	float friction = randomScalar(friction1_, friction2_);
	float alpha = randomScalar(startAlpha1_, startAlpha2_);
	float alphac = randomCounterScalar(alpha, endAlpha1_, endAlpha2_, life);

	Vector velocity;
	randomVector(velocity, velocity1_, velocity2_);
	Vector color, colorc;
	randomVector(color, startColor1_, startColor2_);
	randomCounterVector(colorc, color, endColor1_, endColor2_, life);
	Vector size, sizec;
	randomVector(size, startSize1_, startSize2_);
	randomCounterVector(sizec, size, endSize1_, endSize2_, life);

	particle.setParticle(life, mass, 
		friction, velocity, gravity_,
		color, colorc, 
		size, sizec, 
		alpha, alphac,
		additiveTexture_,
		windAffect_);
}

void ParticleEmitter::emitLinear(int number, 
	Vector &position1, Vector &position2,
	ParticleEngine &engine,
	ParticleRenderer *renderer,
	GLTextureSet *set,
	bool animate)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleLinear);
		if (!particle) return;

		Vector position;
		randomVector(position, position1, position2);

		createDefaultParticle(*particle);
		particle->position_ = position;
		particle->renderer_ = renderer;
		particle->textureCoord_ = int(RAND * 4.0f);
		if (animate)
		{
			particle->textureSet_ = set;
		}
		else
		{
			int index = MIN(int(RAND * (set->getNoTextures() - 1)), set->getNoTextures() - 1);
			particle->texture_ = set->getTexture(index);
		}
	}
}

void ParticleEmitter::emitExplosionRing(int number,
	Vector &position,
	Vector &inAxis,
	ParticleEngine &engine,
	float width,
	GLTextureSet *set,
	bool animate)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleRing);
		if (!particle) return;

		createDefaultParticle(*particle);

		float ang = RAND * 2.0f * 3.14f;
		float speed = width * 4.0f;
		Vector axis = inAxis.Normalize();
		Vector otheraxis(0.0f, 1.0f, 0.0f);
		if (fabs(axis[1]) > 0.7f)
		{
			otheraxis = Vector(1.0f, 0.0f, 0.0f);
		}
		Vector p = axis * otheraxis;
		Vector velocity = VectorLib::rotationAroundAxis(p, ang, axis) * speed;

		particle->velocity_ = velocity;
		particle->position_ = position;
		particle->renderer_ = ParticleRendererQuads::getInstance();
		particle->textureCoord_ = int(RAND * 4.0f);
		if (animate)
		{
			particle->textureSet_ = set;
		}
		else
		{
			int index = MIN(int(RAND * (set->getNoTextures() - 1)), set->getNoTextures() - 1);
			particle->texture_ = set->getTexture(index);
		}
	}
}

void ParticleEmitter::emitDebris(int number,
	Vector &position,
	ParticleEngine &engine)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleDebris);
		if (!particle) return;

		createDefaultParticle(*particle);

		float direction = RAND * 3.14f * 2.0f;
		float speed = RAND * 25.0f + 5.0f;
		float height = RAND * 25.0f + 15.0f;
		Vector velocity(getFastSin(direction) * speed, 
			getFastCos(direction) * speed, height);

		if (RAND > 0.5f)
		{
			DebrisActionRenderer *debris = new DebrisActionRenderer;
			particle->velocity_ = velocity;
			particle->position_ = position;
			particle->renderer_ = ParticleRendererDebris::getInstance();
			particle->userData_ = debris;
		}
		else
		{
			SmokeActionRenderer *smoke = new SmokeActionRenderer;
			particle->velocity_ = velocity;
			particle->position_ = position;
			particle->renderer_ = ParticleRendererSmoke::getInstance();
			particle->userData_ = smoke;
		}
	}
}

void ParticleEmitter::emitSmoke(int number,
	Vector &position,
	ParticleEngine &engine)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleSmoke);
		if (!particle) return;

		createDefaultParticle(*particle);

		particle->position_ = position;
		particle->renderer_ = ParticleRendererQuads::getInstance();
		particle->texture_ = &ExplosionTextures::instance()->smokeTexture;
		particle->shadow_ = (RAND > 0.25f);
		particle->textureCoord_ = (int) (RAND * 4.0f);
	}
}

void ParticleEmitter::emitNapalm(
	Vector &position,
	ParticleEngine &engine,
	GLTextureSet *set)
{
	Particle *particle = engine.getNextAliveParticle(ParticleNapalm);
	if (!particle) return;

	createDefaultParticle(*particle);

	particle->position_ = position;
	particle->renderer_ = ParticleRendererNapalm::getInstance();
	particle->textureCoord_ = 0;
	particle->userData_ = new NapalmRenderer(set);
}

void ParticleEmitter::emitSpray(
	Vector &position,
	ParticleEngine &engine,
	float width,
	GLTexture *texture)
{
	for (int i=0; i<6 + int(width) * 2; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleSpray);
		if (!particle) return;

		createDefaultParticle(*particle);

		float rotation = RAND * 2.0f * 3.14f;
		float x = sinf(rotation);
		float y = cosf(rotation);
		Vector pos = position;
		pos[0] += (x * width * RAND);
		pos[1] += (y * width * RAND);
		Vector velocity;
		velocity[0] = (x * RAND) / 10.0f;
		velocity[1] = (y * RAND) / 10.0f;
		velocity[2] = 25.0f * RAND + 15.0f;

		particle->texture_ = texture;
		particle->velocity_ = velocity;
		particle->position_ = pos;
		particle->renderer_ = ParticleRendererQuads::getInstance();
		particle->textureCoord_ = (int) (RAND * 4.0f);
	}
}

void ParticleEmitter::emitTalk(
	Vector &position,
	ParticleEngine &engine)
{
	Particle *particle = engine.getNextAliveParticle(ParticleTalk);
	if (!particle) return;

	createDefaultParticle(*particle);

	particle->position_ = position;
	particle->renderer_ = ParticleRendererQuads::getInstance();
	particle->textureCoord_ = 0;
	particle->texture_ = &ExplosionTextures::instance()->talkTexture;
}

void ParticleEmitter::emitWallHit(
	Vector &position,
	ParticleEngine &engine,
	OptionsTransient::WallSide type)
{
	Particle *particle = engine.getNextAliveParticle(ParticleWall);
	if (!particle) return;

	createDefaultParticle(*particle);

	particle->position_ = position;
	particle->renderer_ = ParticleRendererWall::getInstance();
	particle->userData_ = new WallActionRenderer(position, type);
}

void ParticleEmitter::emitTransport(
	Vector &position,
	ParticleEngine &engine,
	GLTextureSet *set)
{
	Particle *particle = engine.getNextAliveParticle(ParticleTransport);
	if (!particle) return;

	Vector velocity;
	createDefaultParticle(*particle);

	particle->position_ = position;
	particle->renderer_ = ParticleRendererQuads::getInstance();
	particle->textureCoord_ = 0;
	particle->textureSet_ = set;
}

void ParticleEmitter::emitExplosion(
	Vector &position,
	ParticleEngine &engine,
	float width,
	GLTextureSet *set,
	bool animate)
{
	for (int i=0; i<int(width) * 4; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleExplosion);
		if (!particle) return;

		float randRotXY = (RAND * TWOPI);
		float randRotXZ = (RAND * TWOPI);
		float cosRandRotXZ = (float) cos(randRotXZ);
		Vector velocity;
		velocity[0] = float(sin(randRotXY) * cosRandRotXZ);
		velocity[1] = float(cos(randRotXY) * cosRandRotXZ);
		velocity[2] = float(sin(randRotXZ));
		velocity *= (width * 2.5f);

		float size = RAND * width * 2.0f + 2.0f;

		setEndSize(size, size, size, size);

		createDefaultParticle(*particle);

		particle->velocity_ = velocity;
		particle->position_ = position;
		particle->renderer_ = ParticleRendererQuads::getInstance();
		particle->textureCoord_ = int(RAND * 4.0f);
		if (animate)
		{
			particle->textureSet_ = set;
		}
		else
		{
			int index = MIN(int(RAND * (set->getNoTextures() - 1)), set->getNoTextures() - 1);
			particle->texture_ = set->getTexture(index);
		}
	}
}

void ParticleEmitter::emitMushroom(
	Vector &position,
	ParticleEngine &engine,
	int number,
	float width,
	GLTextureSet *set,
	bool animate)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticleMushroom);
		if (!particle) return;

		createDefaultParticle(*particle);

		particle->position_ = position;
		particle->renderer_ = ParticleRendererMushroom::getInstance();
		particle->textureCoord_ = int(RAND * 4.0f);
		particle->shadow_ = (RAND > 0.80f);
		particle->userData_ = new ExplosionNukeRendererEntry(position, width);

		if (animate)
		{
			particle->textureSet_ = set;
		}
		else
		{
			int index = MIN(int(RAND * (set->getNoTextures() - 1)), set->getNoTextures() - 1);
			particle->texture_ = set->getTexture(index);
		}
	}
}

void ParticleEmitter::emitPrecipitation(
	Vector &position,
	ParticleEngine &engine,
	int number,
	bool rain)
{
	for (int i=0; i<number; i++)
	{
		Particle *particle = engine.getNextAliveParticle(ParticlePrecipitation);
		if (!particle) return;

		createDefaultParticle(*particle);

		if (rain)
		{
			particle->texture_ = &ExplosionTextures::instance()->rainTexture;
			particle->renderer_ = ParticleRendererRain::getInstance();
		}
		else
		{
			particle->texture_ = &ExplosionTextures::instance()->snowTexture;
			particle->renderer_ = ParticleRendererSnow::getInstance();
		}
		particle->position_[0] = position[0] + RAND * 400.0f - 200.0f;
		particle->position_[1] = position[1] + RAND * 400.0f - 200.0f;
		particle->position_[2] = 180.0f;
		particle->shadow_ = false;
	}
}
