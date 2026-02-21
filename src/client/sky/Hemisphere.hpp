////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_Hemisphere_hpp_INCLUDE__
#define __INCLUDE_Hemisphere_hpp_INCLUDE__

class Image;
class Vector;
class Hemisphere  
{
public:
	enum Flags
	{
		eWidthTexture = 1
	};

	static void draw(float radius, float radius2,
		int heightSlices = 10, int rotationSlices = 20,
		int startHeightSlice = 0, int startRotationSlice = 0,
		int endHeightSlice = 10, int endRotationSlice = 10,
		bool inverse = false, unsigned int flags = 0);
	static void drawColored(float radius, float radius2, 
		int heightSlices, int rotationSlices,
		int startHeightSlice, int startRotationSlice,
		int endHeightSlice, int endRotationSlice,
		bool inverse, Image &colors, Vector &sunDir, int daytime, 
		bool horizonGlow);

private:
	Hemisphere();
	virtual ~Hemisphere();
};

#endif // __INCLUDE_Hemisphere_hpp_INCLUDE__
