////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_Water_hpp_INCLUDE__
#define __INCLUDE_Water_hpp_INCLUDE__

#include <common/ProgressCounter.hpp>
#include <common/Vector.hpp>
#include <image/Image.hpp>
#include <GLEXT/GLTexture.hpp>

class Water2;
class Water2Renderer;
class WaterMapPoints;
class WaterWaveDistance;
class WaterWaves;

class Water
{
public:
	Water();
	virtual ~Water();

	void draw();
	void generate( ProgressCounter* counter = nullptr );
	void simulate( float frameTime );
	bool explosion( Vector position, float size );

	// clang-format off
	// uncrustify off
	bool   getWaterOn()      { return waterOn_; }
	float  getWaterHeight()  { return height_; }
	Image& getWaterBitmap()  { return bitmapWater_; }
	float* getIndexErrors();
	float  getWaveDistance( int x, int y );
	void   setTransparency( float transparency );
	float  getTransparency() { return transparency_; }
	// uncrustify on
	// clang-format on

	void bindWaterReflection();
	void unBindWaterReflection();
	void drawPoints();

	GLTexture& getReflectionTexture();

protected:
	float              height_;
	float              transparency_;
	float              maxTransparency_;
	bool               waterOn_;
	Water2*            wMap_;
	Water2Renderer*    wTex_;
	WaterMapPoints*    wMapPoints_;
	WaterWaveDistance* wWaveDistance_;
	WaterWaves*        waves_;
	GLTexture          landTexWater_;
	Image              bitmapWater_;
};

#endif  // __INCLUDE_Water_hpp_INCLUDE__
