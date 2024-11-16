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

#if !defined(__INCLUDE_DefaultTankRendererh_INCLUDE__)
#define __INCLUDE_DefaultTankRendererh_INCLUDE__

#include <tank/Tank.h>
#include <tank/TankModel.h>
#include <tankgraph/ModelRendererTank.h>
#include <tankgraph/TargetRendererImpl.h>
#include <GLW/GLWTankTip.h>
#include <GLEXT/GLState.h>

class ModelRendererSimulator;
class TargetRendererImplTankAIM
{
public:
	static void simulate(float frameTime) { timeLeft_ -= frameTime; }

	static ModelRendererSimulator *getAutoAimModel();
	static bool drawAim() { return (timeLeft_ > 0.0f); }
	static void setAimPosition(Vector &pos) { aimPosition_ = pos; timeLeft_= 5.0f; }
	static Vector &getAimPosition() { return aimPosition_; }

	static Vector aimPosition_;

protected:
	static float timeLeft_;
};

class TargetRendererImplTankHUD
{
public:
	static void simulate(float frameTime) { timeLeft_ -= frameTime; }

	static bool drawText() { return (timeLeft_ > 0.0f); }
	static void setText(const char *textA, const char *textB, float percentage = -1.0f)
		{ textA_ = textA; textB_ = textB; percentage_ = percentage; timeLeft_= 2.0f; }
	static const char *getTextA() { return textA_.c_str(); }
	static const char *getTextB() { return textB_.c_str(); }
	static float getPercentage() { return percentage_; }

protected:
	static float timeLeft_;
	static float percentage_;
	static std::string textA_;
	static std::string textB_;

};

class TargetRendererImplTank : public TargetRendererImpl
{
public:
	TargetRendererImplTank(Tank *tank);
	virtual ~TargetRendererImplTank();

	// TargetRendererImpl
	virtual void simulate(float frameTime);
	virtual void drawParticle(float distance);

	void render(float distance);
	void renderReflection(float distance);
	void renderShadow(float distance);
	void render2D(float distance);

	// TargetRenderer
	virtual void fired();
	virtual void shieldHit();
	virtual void targetBurnt() {}

	void resetModel() { mesh_ = 0; }
	ModelRendererTank *getMesh();
	GLWTankTips *getTips() { return &tankTips_; }

protected:
	Tank *tank_;
	ModelRendererTank *mesh_;
	GLWTankTips tankTips_;
	float fireOffSet_;
	float shieldHit_;
	float smokeTime_, smokeWaitForTime_;
	float totalTime_;
	float frame_;

	void drawNames();
	void drawLife();
	void drawLifeBar(Vector &bilX, float value, float height, float barheight);
	void drawSight();
	void drawOldSight();
	void drawArrow();
};


#endif
