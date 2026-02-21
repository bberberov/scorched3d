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

#ifndef __INCLUDE_LargeHemisphere_hpp_INCLUDE__
#define __INCLUDE_LargeHemisphere_hpp_INCLUDE__

#include <list>
#include <common/Vector.hpp>

class Image;
class LargeHemisphere  
{
public:
	LargeHemisphere();
	~LargeHemisphere();

	void clear();

	void draw(float radius, float radius2, 
		unsigned int flags = 0);
	void drawColored(float radius, float radius2, 
		Image &colors, Vector &sunDir, int daytime, 
		bool horizonGlow);

private:
	struct Entry
	{
		unsigned int listNo_;
	};

	std::list<Entry> entries_;
};

#endif // __INCLUDE_LargeHemisphere_hpp_INCLUDE__
