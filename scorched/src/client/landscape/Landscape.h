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

#if !defined(__INCLUDE_Landscapeh_INCLUDE__)
#define __INCLUDE_Landscapeh_INCLUDE__

#include <GLEXT/GLTexture.h>
#include <GLEXT/GLShadowFrameBuffer.h>

class Wall;
class ShadowMap;
class Smoke;
class ProgressCounter;
class Water;
class LandscapePoints;
class Sky;
class GLSLShaderSetup;
class Landscape
{
public:
	static Landscape *instance();

	enum LandscapeTextureType
	{
		eDefault,
		eOther
	};

	void generate(ProgressCounter *counter = 0);
	void recalculateLandscape();
	void recalculateRoof();
	void restoreLandscapeTexture();

	// Access to internal objects
	Smoke &getSmoke() { return *smoke_; }
	ShadowMap &getShadowMap();
	Wall &getWall() { return *wall_; }
	Sky &getSky() { return *sky_; }
	Water &getWater() { return *water_; }
	LandscapePoints &getPoints() { return *points_; }

	// Textures created during landscape texturing
	Image &getMainMap() { return mainMap_; }
	Image &getScorchMap() { return scorchMap_; }
	GLTexture &getMainTexture() { return texture_; }
	GLTexture &getMagTexture() { return magTexture_; }
	GLTexture &getPlanATexture() { return planAlphaTexture_; }
	GLTexture &getPlanTexture() { return planTexture_; }
	GLTexture &getLandscapeTexture1() { return landTex1_; }
	GLTexture &getGroundTexture() { return groundTexture_; }
	GLTexture &getDetailTexture() { return detailTexture_; }
	GLTexture &getRoofTexture() { return roofTexture_; }
	GLTexture &getArenaMainTexture() { return arenaMainTexture_; }

	LandscapeTextureType getTextureType() { return textureType_; }
	void setTextureType(LandscapeTextureType type) { textureType_ = type; }

	float *getShadowTextureMatrix() { return shadowTextureMatrix_; }
	GLShadowFrameBuffer &getShadowFrameBuffer() { return shadowFrameBuffer_; }

	void updatePlanTexture();
	void updatePlanATexture();
	int getPlanTexSize();
	int getMapTexSize();

	unsigned int getChangeCount() { return changeCount_; }

	void drawTearDown();
	void drawSetup();
	void drawLand();
	void drawWater();
	void drawObjects();
	void drawShadows();
	void calculateVisibility();

	void simulate(float frameTime);

protected:
	static Landscape *instance_;

	// All objects that are used to draw the scene
	Wall *wall_;
	Sky *sky_;
	Smoke *smoke_;
	Water *water_;
	LandscapeTextureType textureType_;
	LandscapePoints *points_;

	// Textures used for landscape
	GLTexture texture_;
	GLTexture magTexture_;
	GLTexture planTexture_;
	GLTexture planAlphaTexture_;
	GLTexture detailTexture_;
	GLTexture roofTexture_;
	GLTexture landTex1_;
	GLTexture groundTexture_;
	GLTexture arenaMainTexture_;
	GLTexture arenaSurroundTexture_;
	Image mainMap_;
	Image scorchMap_;
	Image bitmapPlanAlphaAlpha_;
	Image bitmapPlanAlpha_;
	Image bitmapPlan_;

	// Shadow map
	float shadowTextureMatrix_[16];
	float lightModelMatrix_[16];
	float lightProjMatrix_[16];
	GLShadowFrameBuffer shadowFrameBuffer_;
	GLSLShaderSetup *landShader_;
	GLTexture colorDepthMap_;

	// Variables used to set when the water is refreshed
	bool resetLandscape_, resetRoof_;
	float resetLandscapeTimer_, resetRoofTimer_;
	unsigned int changeCount_;

	void savePlan();
	void actualDrawLandTextured();
	void actualDrawLandReflection();
	void actualDrawLandShader();
	void createShadowMatrix();
	void drawGraphicalTextureMap(GLTexture &texture);

	// Nasty, we really need some kind of viewport/rendering context
	// that the current rendering state for the scene can be stored.
	// Useful for things that relate to each drawn scene rather than
	// global. e.g. camera window vs main window
	struct CameraContext
	{
		CameraContext();

		ShadowMap *shadowMap_;
	} cameraContexts_[2];

private:
	Landscape();
	virtual ~Landscape();
};


#endif
