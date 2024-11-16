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

#include <tankgraph/TargetRendererImplTarget.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <landscape/Landscape.h>
#include <landscape/ShadowMap.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLState.h>
#include <client/ScorchedClient.h>
#include <engine/ActionController.h>
#include <graph/ModelRendererStore.h>
#include <graph/OptionsDisplay.h>
#include <float.h>

TargetRendererImplTarget::TargetRendererImplTarget(Target *target,
	ModelID model, ModelID burntModel, 
	float scale, float color, bool billboard) :
	TargetRendererImpl(target),
	modelId_(model), burntModelId_(burntModel),
	target_(target),
	burnt_(false),
	shieldHit_(0.0f), totalTime_(0.0f),
	targetTips_(target),
	scale_(scale), color_(color), billboard_(billboard)
{
	modelRenderer_ = new ModelRendererSimulator(
		ModelRendererStore::instance()->loadModel(model));
	burntModelRenderer_ = new ModelRendererSimulator(
		ModelRendererStore::instance()->loadModel(burntModel));

	if (burntModelId_.getType()[0] == 'T' ||
		modelId_.getType()[0] == 'T')
	{
		tree_ = true;
	}
}

TargetRendererImplTarget::~TargetRendererImplTarget()
{
	delete modelRenderer_;
	delete burntModelRenderer_;
}

void TargetRendererImplTarget::simulate(float frameTime)
{
	totalTime_ += frameTime;
	if (shieldHit_ > 0.0f)
	{
		shieldHit_ -= frameTime / 25.0f;
		if (shieldHit_ < 0.0f) shieldHit_ = 0.0f;
	}

	if (burnt_) burntModelRenderer_->simulate(frameTime * 20.0f);
	else modelRenderer_->simulate(frameTime * 20.0f);
}

void TargetRendererImplTarget::cacheMatrix()
{
	// Generate and cache the OpenGL transform matrix
	if (!matrixCached_)
	{
		cachedMatrix_.identity();
		cachedMatrix_.translate(
			target_->getLife().getFloatPosition()[0], 
			target_->getLife().getFloatPosition()[1], 
			target_->getLife().getFloatPosition()[2]);
		cachedMatrix_.multiply(target_->getLife().getFloatRotMatrix());
		cachedMatrix_.scale(scale_, scale_, scale_);

		matrixCached_ = true;
	}
}

void TargetRendererImplTarget::render(float distance)
{
	createParticle();
	float size = 2.0f;
	if (!tree_)
	{
		storeTarget2DPos();
		size = getTargetSize();

		// Draw texture shadows (if hardware shadows aren't on)
		if (target_->getTargetState().getDisplayShadow() &&
			Landscape::instance()->getShadowMap().shouldAddShadow())
		{
			Landscape::instance()->getShadowMap().addCircle(
				target_->getLife().getFloatPosition()[0], 
				target_->getLife().getFloatPosition()[1], 
				target_->getLife().getSize().Max().asFloat() + 2.0f,
				1.0f);
		}
	}

	// Draw the target model
	float drawCullingDistance = OptionsDisplay::instance()->getDrawCullingDistance() * size;
	if (distance < drawCullingDistance)
	{
		glColor3f(color_, color_, color_);
		glPushMatrix();
			if (billboard_)
			{
				glTranslatef(
					target_->getLife().getFloatPosition()[0], 
					target_->getLife().getFloatPosition()[1], 
					target_->getLife().getFloatPosition()[2]);
				glMultMatrixf(GLCameraFrustum::instance()->getBilboardMatrix());
				glScalef(scale_, scale_, scale_);
			}
			else
			{
				 cacheMatrix();
				 glMultMatrixf(cachedMatrix_);
			}

			if (burnt_) burntModelRenderer_->drawBottomAligned(distance, 1.0f);
			else modelRenderer_->drawBottomAligned(distance, 1.0f);
		glPopMatrix();
	}
}

void TargetRendererImplTarget::renderReflection(float distance)
{
	// Draw the target model
	float drawCullingDistance = OptionsDisplay::instance()->getDrawCullingDistance() * 2.0f;
	if (distance < drawCullingDistance)
	{
		cacheMatrix();
		glColor3f(color_, color_, color_);
		glPushMatrix();
			if (billboard_)
			{
				glTranslatef(
					target_->getLife().getFloatPosition()[0], 
					target_->getLife().getFloatPosition()[1], 
					target_->getLife().getFloatPosition()[2]);
				glMultMatrixf(GLCameraFrustum::instance()->getBilboardMatrix());
				glScalef(scale_, scale_, scale_);
			}
			else
			{
				 glMultMatrixf(cachedMatrix_);
			}

			if (burnt_) burntModelRenderer_->drawBottomAligned(distance, 1.0f);
			else modelRenderer_->drawBottomAligned(distance, 1.0f);
		glPopMatrix();
	}
}

void TargetRendererImplTarget::render2D(float distance)
{
	// Add the tooltip that displays the tank info
	GLWToolTip::instance()->addToolTip(&targetTips_.targetTip,
		float(posX_) - 10.0f, float(posY_) - 10.0f, 20.0f, 20.0f);
}

void TargetRendererImplTarget::renderShadow(float distance)
{
	if (billboard_) return;

	glPushMatrix();
		glMultMatrixf(cachedMatrix_);
		if (burnt_) burntModelRenderer_->drawBottomAligned(distance, 1.0f, false);
		else modelRenderer_->drawBottomAligned(distance, 1.0f, false);
	glPopMatrix();
}

void TargetRendererImplTarget::drawParticle(float distance)
{
	if (!getVisible()) return;

	drawParachute();
	drawShield(shieldHit_, totalTime_);
}

void TargetRendererImplTarget::shieldHit()
{
	shieldHit_ = 0.25f;
}

void TargetRendererImplTarget::fired()
{
}

void TargetRendererImplTarget::targetBurnt()
{
	burnt_ = true;
}
