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

#if !defined(__INCLUDE_TargetRendererImplTargeth_INCLUDE__)
#define __INCLUDE_TargetRendererImplTargeth_INCLUDE__

#include <tankgraph/TargetRendererImpl.h>
#include <graph/ModelRendererSimulator.h>
#include <common/ModelID.h>
#include <GLW/GLWTankTip.h>

class TargetRendererImplTarget : 
	public TargetRendererImpl
{
public:
	TargetRendererImplTarget(Target *target, 
		ModelID model, ModelID burntModel,
		float scale, float color, bool billboard);
	virtual ~TargetRendererImplTarget();

	// TargetRendererImpl
	virtual void drawParticle(float distance);
	virtual void simulate(float frameTime);

	void render(float distance);
	void renderReflection(float distance);
	void renderShadow(float distance);
	void render2D(float distance);

	// TargetRenderer
	virtual void shieldHit();
	virtual void fired();
	virtual void targetBurnt();

	GLWTargetTips &getTips() { return targetTips_; }
	ModelID &getModelId() { return modelId_; }

protected:
	ModelID modelId_, burntModelId_;
	Target *target_;
	ModelRendererSimulator *modelRenderer_;
	ModelRendererSimulator *burntModelRenderer_;
	bool burnt_, billboard_;
	float shieldHit_, totalTime_;
	float scale_, color_;
	GLWTargetTips targetTips_;

	void cacheMatrix();
};

#endif // __INCLUDE_TargetRendererImplTargeth_INCLUDE__
