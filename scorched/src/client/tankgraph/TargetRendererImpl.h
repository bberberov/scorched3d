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

#if !defined(__INCLUDE_TargetRendererImplh_INCLUDE__)
#define __INCLUDE_TargetRendererImplh_INCLUDE__

#include <target/Target.h>
#include <target/TargetRenderer.h>
#include <common/Matrix16.h>

class TargetVisibilityPatch;
class RenderObjectLists;
class TargetRendererImpl : public TargetRenderer
{
public:
	TargetRendererImpl(Target *target);
	virtual ~TargetRendererImpl();

	// Interface
	virtual void drawParticle(float distance) = 0;
	virtual void simulate(float frameTime) = 0;
	virtual void moved();

	// Particles
	void setMakeParticle() { particleMade_ = false; }

	// Highlight
	enum HighlightType
	{
		eNoHighlight,
		ePlayerHighlight,
		eOtherHighlight
	};
	static void setHighlightType(HighlightType type) { highlightType_ = type; }
	void createParticle();

protected:
	static HighlightType highlightType_;
	bool tree_, matrixCached_;
	Matrix16 cachedMatrix_;
	Target *target_;
	TargetVisibilityPatch *currentVisibilityPatch_;
	int patchEpoc_;
	bool particleMade_;

	void drawShield(float shieldHit, float totalTime);
	void drawParachute();

	bool getVisible();

	float getTargetSize();
	float getTargetFade(float distance, float size);
	void storeTarget2DPos();

	void setMovedPatch(TargetVisibilityPatch *newPatch);

	double posX_, posY_, posZ_;
};

#endif // __INCLUDE_TargetRendererImplh_INCLUDE__
