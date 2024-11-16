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

#if !defined(__INCLUDE_Water2Rendererh_INCLUDE__)
#define __INCLUDE_Water2Rendererh_INCLUDE__

#include <GLEXT/GLTexture.h>
#include <GLEXT/GLFrameBufferObject.h>
#include <GLSL/GLSLShaderSetup.h>
#include <common/Vector.h>

class Water2;
class Water2Patches;
class WaterWaves;
class WaterMapPoints;
class LandscapeTexBorderWater;
class ProgressCounter;
class Water2Renderer
{
public:
	Water2Renderer();
	~Water2Renderer();

	void draw(Water2 &water2, WaterMapPoints &points, WaterWaves &waves, float transparency);
	void simulate(float frameTime);
	void generate(LandscapeTexBorderWater *water, ProgressCounter *counter = 0);

	void bindWaterReflection() { reflectionBuffer_.bind(); }
	void unBindWaterReflection() { reflectionBuffer_.unBind(); }
	void drawPoints(WaterMapPoints &points);
	GLTexture &getReflectionTexture() { return reflectionTexture_; }

protected:
	float totalTime_;
	float waterHeight_;
	GLTexture reflectionTexture_;
	GLTexture normalTexture_;
	GLTextureBase *noShaderWaterTexture_;
	GLFrameBufferObject reflectionBuffer_;
	Vector landscapeSize_;
	Vector windDir1_, windDir2_;
	float windSpeed1_, windSpeed2_;

	Water2Patches *currentPatch_;
	GLSLShaderSetup *waterShader_;

	void drawWaterShaders(Water2 &water2, float transparency);
	void drawWaterNoShaders(Water2 &water2, float transparency);
	void drawWater(Water2 &water2, GLSLShaderSetup *waterShader);
};

#endif // __INCLUDE_Water2Rendererh_INCLUDE__
