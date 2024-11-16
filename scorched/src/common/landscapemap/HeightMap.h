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

#if !defined(AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)
#define AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_

#include <stdlib.h>
#include <common/ProgressCounter.h>
#include <common/FixedVector.h>
#include <common/Vector.h>

class Line;
class GraphicalHeightMap;
class HeightMap  
{
public:
	HeightMap();
	virtual ~HeightMap();

	void create(const int width, const int height, bool invertedNormals);
	void reset();

	// Height map size fns
	int getMapWidth() { return width_; }
	int getMapHeight() { return height_; }

	// Get height fns (z values)
	inline fixed getHeight(int w, int h) { 
		if (w >= 0 && h >= 0 && w<=width_ && h<=height_) 
			return heightData_[(width_+1) * h + w].position[2]; 
		return fixed(0); }
	fixed getInterpHeight(fixed w, fixed h);

	// Get normal functions
	FixedVector &getNormal(int w, int h);
	void getInterpNormal(fixed w, fixed h, FixedVector &normal);

	bool getIntersect(Line &direction, Vector &intersect);

	// Alters the actual internal HeightMap points
	void setHeight(int w, int h, fixed height);

	GraphicalHeightMap *getGraphicalMap() { return graphicalMap_; }
	void setGraphicalMap(GraphicalHeightMap *map) { graphicalMap_ = map; }

protected:
	struct HeightData
	{
		FixedVector position;
		FixedVector normal;
	};

	bool invertedNormals_;
	int width_, height_;
	HeightData *heightData_;
	GraphicalHeightMap *graphicalMap_;

	bool getVector(FixedVector &vec, int x, int y);
	void getVectorPos(int pos, int &x, int &y, int dist=1);
};

#endif // !defined(AFX_HEIGHTMAP_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)

