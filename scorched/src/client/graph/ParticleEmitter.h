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

#if !defined(__INCLUDE_ParticleEmitterh_INCLUDE__)
#define __INCLUDE_ParticleEmitterh_INCLUDE__

#include <graph/ParticleEngine.h>
#include <GLEXT/GLTextureSet.h>
#include <common/OptionsTransient.h>

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	float life1_, life2_;
	float mass1_, mass2_;
	float friction1_, friction2_;

	float startAlpha1_, startAlpha2_;
	float endAlpha1_, endAlpha2_;
	Vector velocity1_, velocity2_;
	Vector startColor1_, startColor2_;
	Vector endColor1_, endColor2_;
	Vector startSize1_, startSize2_;
	Vector endSize1_, endSize2_;
	Vector gravity_;
	bool additiveTexture_;
	bool windAffect_;

	void createDefaultParticle(Particle &particle);

	void setAttributes(
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
		bool windAffect);

	void setLife(float life1, float life2);
	void setMass(float mass1, float mass2);
	void setFriction(float friction1, float friction2);
	void setVelocity(Vector velocity1, Vector velocity2);
	void setStartColor(
		Vector startColor1, float startAlpha1,
		Vector startColor2, float startAlpha2);
	void setEndColor(
		Vector endColor1, float endAlpha1,
		Vector endColor2, float endAlpha2);
	void setStartSize(
		float startX1, float startY1,
		float startX2, float startY2);
	void setEndSize(
		float endX1, float endY1,
		float endX2, float endY2);
	void setGravity(Vector gravity);
	void setAdditiveTexture(bool additiveTexture);
	void setWindAffect(bool windAffect);

	void emitLinear(int number, 
		Vector &position1, Vector &position2,
		ParticleEngine &engine,
		ParticleRenderer *renderer,
		GLTextureSet *set,
		bool animate);
	void emitExplosionRing(int number,
		Vector &position,
		Vector &axis,
		ParticleEngine &engine,
		float width,
		GLTextureSet *set,
		bool animate);
	void emitDebris(int number,
		Vector &position,
		ParticleEngine &engine);
	void emitSmoke(int number,
		Vector &position,
		ParticleEngine &engine);
	void emitNapalm(
		Vector &position,
		ParticleEngine &engine,
		GLTextureSet *set);
	void emitSpray(
		Vector &position,
		ParticleEngine &engine,
		float width,
		GLTexture *texture);
	void emitTalk(
		Vector &position,
		ParticleEngine &engine);
	void emitWallHit(
		Vector &position,
		ParticleEngine &engine,
		OptionsTransient::WallSide type);
	void emitTransport(
		Vector &position,
		ParticleEngine &engine,
		GLTextureSet *set);
	void emitExplosion(
		Vector &position,
		ParticleEngine &engine,
		float width,
		GLTextureSet *set,
		bool animate);
	void emitMushroom(
		Vector &position,
		ParticleEngine &engine,
		int number,
		float width,
		GLTextureSet *set,
		bool animate);
	void emitPrecipitation(
		Vector &position,
		ParticleEngine &engine,
		int number,
		bool rain);
};

#endif // __INCLUDE_ParticleEmitterh_INCLUDE__

