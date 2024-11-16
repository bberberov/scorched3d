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

#include <tankgraph/RenderTargets.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <tankgraph/TargetRendererImplTarget.h>
#include <tankgraph/RenderTracer.h>
#include <tankgraph/RenderGeoms.h>
#include <graph/OptionsDisplay.h>
#include <graph/ModelRendererTree.h>
#include <target/TargetContainer.h>
#include <target/TargetState.h>
#include <target/TargetLife.h>
#include <client/ScorchedClient.h>
#include <common/OptionsScorched.h>
#include <engine/Simulator.h>
#include <GLEXT/GLGlobalState.h>
#include <landscape/Landscape.h>
#include <water/Water.h>
#include <land/VisibilityPatchGrid.h>
#include <sky/Sky.h>
#include <algorithm>

RenderTargets *RenderTargets::instance_ = 0;

RenderTargets *RenderTargets::instance()
{
	if (!instance_)
	{
		instance_ = new RenderTargets;
	}
	return instance_;
}

RenderTargets::RenderTargets() :
	treesDrawn_(0), targetsDrawn_(0)
{
}

RenderTargets::~RenderTargets()
{
}

void RenderTargets::Renderer2D::draw(const unsigned state)
{
	RenderTargets::instance()->draw2d();
}

void RenderTargets::Renderer2D::simulate(const unsigned state, float simTime)
{
	// Simulate the HUD
	TargetRendererImplTankHUD::simulate(simTime);
	TargetRendererImplTankAIM::simulate(simTime);
}

void RenderTargets::Renderer3D::draw(const unsigned state)
{
	RenderTracer::instance()->draw(state);
	RenderGeoms::instance()->draw(state);
	RenderTargets::instance()->draw(false);
}

void RenderTargets::Renderer3D::simulate(const unsigned state, float simTime)
{
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();
	stepTime += simTime;

	// step size = 1.0 / physics fps = steps per second
	const float stepSize = 0.02f;
	if (stepTime >= stepSize)
	{
		float time = stepTime * ScorchedClient::instance()->getSimulator().getFast().asFloat();

		void *currentPatchPtr = 0, *currentObject = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTargets());
			while (currentObject = itor.getNext())
			{
				Target *target = (Target *) currentObject;
				TargetRendererImpl *renderImpl = (TargetRendererImpl *) target->getRenderer();
				renderImpl->simulate(time);
			}
		}

		itor.init(TargetVisibilityPatch::getLargeTargets());
		while (currentObject = itor.getNext())
		{
			Target *target = (Target *) currentObject;
			TargetRendererImpl *renderImpl = (TargetRendererImpl *) target->getRenderer();
			renderImpl->simulate(time);
		}

		stepTime = 0.0f;
	}
}

void RenderTargets::Renderer3D::enterState(const unsigned state)
{

}

static void drawTargetShadows(TargetVisibilityIterator &itor, float distance) 
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (target->getTargetState().getDisplayHardwareShadow())
		{
			if (target->getType() != Target::TypeTank)
			{
				TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
				renderImpl->renderShadow(distance);
			}
			else
			{
				TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
				renderImpl->renderShadow(distance);
			}
		}
	}
}

void RenderTargets::shadowDraw()
{
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
	{
		unsigned int wantedstate = GLState::BLEND_OFF | 
			GLState::ALPHATEST_OFF | GLState::TEXTURE_OFF;
		GLState glstate(wantedstate);

		{
			GLGlobalState globalState(0);
			ModelRendererTree::setSkipPre(true);
			ModelRendererTree::drawInternalPre(false);

			void *currentPatchPtr = 0;
			TargetListIterator patchItor(patchInfo.getTreeVisibility());
			TargetVisibilityIterator itor;
			while (currentPatchPtr = patchItor.getNext())
			{
				TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

				itor.init(currentPatch->getTrees());
				drawTargetShadows(itor, currentPatch->getDistance());
			}

			ModelRendererTree::setSkipPre(false);
		}
		{
			GLGlobalState globalState(0);
			void *currentPatchPtr = 0;
			TargetListIterator patchItor(patchInfo.getTargetVisibility());
			TargetVisibilityIterator itor;
			while (currentPatchPtr = patchItor.getNext())
			{
				TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

				itor.init(currentPatch->getTargets());
				drawTargetShadows(itor, currentPatch->getDistance());
			}
		}

		{
			GLGlobalState globalState(0);
			TargetVisibilityIterator itor;

			itor.init(TargetVisibilityPatch::getLargeTargets());
			drawTargetShadows(itor, 0.0f);
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_OBJ");
}

static void drawTargets(TargetVisibilityIterator &itor, float distance, bool reflection, fixed waterHeight)
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (reflection)
		{
			if (target->getLife().getTargetPosition()[2] < waterHeight)
			{
				continue;
			}
		}

		if (target->getType() != Target::TypeTank)
		{
			TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
			if (reflection)	renderImpl->renderReflection(distance);
			else renderImpl->render(distance);
		}
		else
		{
			TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
			if (reflection)	renderImpl->renderReflection(distance);
			else renderImpl->render(distance);
		}
	}
}

void RenderTargets::draw(bool reflection)
{
	fixed waterHeight = -100;
	if (Landscape::instance()->getWater().getWaterOn())
	{
		waterHeight = fixed::fromFloat(Landscape::instance()->getWater().getWaterHeight());
	}

	if (!reflection)
	{
		if (!OptionsDisplay::instance()->getNoFog())
		{
			glEnable(GL_FOG); // NOTE: Fog on
		}
	}

	// Don't put fully transparent areas into the depth buffer
	unsigned int wantedstate = GLState::BLEND_ON | 
		GLState::ALPHATEST_ON | GLState::TEXTURE_ON | 
		GLState::NORMALIZE_ON | GLState::LIGHTING_ON | 
		GLState::LIGHT1_ON;
	GLState glstate(wantedstate);
	Landscape::instance()->getSky().getSun().setLightPosition(false);

	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	// Trees
	treesDrawn_ = 0;
	if (!OptionsDisplay::instance()->getNoTrees())
	{
		if (!reflection)
		{
			GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_TREES");
		}

		GLGlobalState globalState(0);
		ModelRendererTree::setSkipPre(true);
		ModelRendererTree::drawInternalPre(true);

		void *currentPatchPtr = 0;
		TargetListIterator patchItor(patchInfo.getTreeVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTrees());
			drawTargets(itor, currentPatch->getDistance(), reflection, waterHeight);

			treesDrawn_+=(unsigned int) currentPatch->getTrees().size();
		}

		ModelRendererTree::setSkipPre(false);

		if (!reflection)
		{
			GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_TREES");
		}
	}

	// Models
	targetsDrawn_ = 0;
	if (!OptionsDisplay::instance()->getNoTargets())
	{
		if (!reflection)
		{
			GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_MODELS");
		}

		GLGlobalState globalState(0);

		void *currentPatchPtr = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTargets());
			drawTargets(itor, currentPatch->getDistance(), reflection, waterHeight);

			targetsDrawn_+=(unsigned int) currentPatch->getTargets().size();
		}

		{
			itor.init(TargetVisibilityPatch::getLargeTargets());
			drawTargets(itor, 0.0f, reflection, waterHeight);

			targetsDrawn_+=(unsigned int) TargetVisibilityPatch::getLargeTargets().size();
		}

		if (!reflection)
		{
			GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "TARGETS_DRAW_MODELS");
		}
	}
}

static void drawTargets2D(TargetVisibilityIterator &itor, float distance)
{
	void *currentObject = 0;
	while (currentObject = itor.getNext())
	{
		Target *target = (Target *) currentObject;
		if (target->getType() != Target::TypeTank)
		{
			TargetRendererImplTarget *renderImpl = (TargetRendererImplTarget *) target->getRenderer();
			renderImpl->render2D(distance);
		}
		else
		{
			TargetRendererImplTank *renderImpl = (TargetRendererImplTank *) target->getRenderer();
			renderImpl->render2D(distance);
		}
	}

	glDisable(GL_FOG); // NOTE: Fog off
}

void RenderTargets::draw2d()
{
	VisibilityPatchInfo &patchInfo = VisibilityPatchGrid::instance()->getPatchInfo();

	// 2D
	{
		void *currentPatchPtr = 0;
		TargetListIterator patchItor(patchInfo.getTargetVisibility());
		TargetVisibilityIterator itor;
		while (currentPatchPtr = patchItor.getNext())
		{
			TargetVisibilityPatch *currentPatch = (TargetVisibilityPatch *) currentPatchPtr;

			itor.init(currentPatch->getTooltips());
			drawTargets2D(itor, currentPatch->getDistance());
		}
	}
}
