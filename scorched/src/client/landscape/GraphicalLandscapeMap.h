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

#if !defined(AFX_GraphicalLandscapeMap_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)
#define AFX_GraphicalLandscapeMap_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_

#include <landscapemap/GraphicalHeightMap.h>
#include <common/Vector.h>

class GLVertexBufferObject;
class GraphicalLandscapeMap  : public GraphicalHeightMap
{
public:
	struct HeightData
	{
		Vector floatPosition;
		Vector floatNormal;
		float texCoord1x, texCoord1y;
		float texCoord2x, texCoord2y;
	};

	GraphicalLandscapeMap();
	virtual ~GraphicalLandscapeMap();

	void create(int width, int height);
	void reset();

	void updateWholeBuffer();

	virtual void setHeight(int w, int h, float height);
	virtual void setNormal(int w, int h, Vector &normal);

	int getMapWidth() { return width_; }
	int getMapHeight() { return height_; }

	HeightData *getHeightData() { return heightData_; }
	GLVertexBufferObject *getBufferObject() { return bufferObject_; }

protected:
	int width_, height_;
	int bufferSizeBytes_;
	HeightData *heightData_;
	GLVertexBufferObject *bufferObject_;
};

#endif // !defined(AFX_GraphicalLandscapeMap_H__F4CB4CAD_C592_4183_AFB2_016FC66C144A__INCLUDED_)

