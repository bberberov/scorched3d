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

#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLInfo.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <land/LandSurround.h>
#include <sky/Sky.h>

LandSurround::LandSurround() :
	listNo_(0)
{

}

LandSurround::~LandSurround()
{

}

void LandSurround::draw(HeightMap &map, bool detail, bool lightMap)
{
	if (!listNo_)
	{
		glNewList(listNo_ = glGenLists(1), GL_COMPILE);
			generateList(map,detail, lightMap);
		glEndList();
	}
	
	glCallList(listNo_);

	GLInfo::addNoTriangles(8);
}

void LandSurround::generate()
{
	if (listNo_) glDeleteLists(listNo_, 1);
	listNo_ = 0;
}

void LandSurround::generateVerts(HeightMap &map)
{
	int width = 1536 + map.getMapWidth();
	int height = 1536 + map.getMapHeight();
	Vector centre(map.getMapWidth() / 2, map.getMapHeight() / 2, 0);
	Vector offset(map.getMapWidth() + width, map.getMapHeight() + height, 0);

	Vector offset2(map.getMapWidth() / 2, map.getMapHeight() / 2, 0);
	hMapBoxVerts_[0] = Vector(centre[0] - offset2[0], centre[1] - offset2[1], 0.0f);
	hMapBoxVerts_[1] = Vector(centre[0] - offset2[0], centre[1] + offset2[1], 0.0f);
	hMapBoxVerts_[2] = Vector(centre[0] + offset2[0], centre[1] + offset2[1], 0.0f);
	hMapBoxVerts_[3] = Vector(centre[0] + offset2[0], centre[1] - offset2[1], 0.0f);

	Vector offset3(width + map.getMapWidth() * 2, height + map.getMapWidth() * 2, 0);
	hMapBoxVerts_[4] = Vector(centre[0] - offset3[0], centre[1] - offset3[1], 0.0f);
	hMapBoxVerts_[5] = Vector(centre[0] - offset3[0], centre[1] + offset3[1], 0.0f);
	hMapBoxVerts_[6] = Vector(centre[0] + offset3[0], centre[1] + offset3[1], 0.0f);
	hMapBoxVerts_[7] = Vector(centre[0] + offset3[0], centre[1] - offset3[1], 0.0f);

	hMapBoxVerts_[8] = Vector(centre[0] - offset2[0], centre[1] - offset3[1], 0.0f);
	hMapBoxVerts_[9] = Vector(centre[0] - offset2[0], centre[1] + offset3[1], 0.0f);
	hMapBoxVerts_[10] = Vector(centre[0] + offset2[0], centre[1] + offset3[1], 0.0f);
	hMapBoxVerts_[11] = Vector(centre[0] + offset2[0], centre[1] - offset3[1], 0.0f);

	hMapBoxVerts_[12] = Vector(centre[0] - offset3[0], centre[1] - offset2[1], 0.0f);
	hMapBoxVerts_[13] = Vector(centre[0] - offset3[0], centre[1] + offset2[1], 0.0f);
	hMapBoxVerts_[14] = Vector(centre[0] + offset3[0], centre[1] + offset2[1], 0.0f);
	hMapBoxVerts_[15] = Vector(centre[0] + offset3[0], centre[1] - offset2[1], 0.0f);
}

void LandSurround::generateList(HeightMap &map, bool detail, bool lightMap)
{
	generateVerts(map);

	const int dataOfs[8][4] = {
		{8,11,3,0},
		{1,2,10,9},
		{4,8,0,12},
		{11,7,15,3},
		{3,15,14,2},
		{2,14,6,10},
		{13,1,9,5},
		{12,0,1,13}
	};

	LandscapeTex &tex =
		*ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();
	Vector &ambient = tex.skyambience;
	Vector &diffuse = tex.skydiffuse;
	Vector &sunPos =
		Landscape::instance()->getSky().getSun().getPosition();
	Vector normal(0.0f, 0.0f, 1.0f);

	float width = float(Landscape::instance()->getGroundTexture().getWidth());
	float height = float(Landscape::instance()->getGroundTexture().getHeight());
	float texTileX = width / 16.0f;
	float texTileY = height / 16.0f;
	
	glBegin(GL_QUADS);
	for (int i=0; i<8; i++) 
	{
		for (int j=0; j<4; j++) 
		{
			Vector pos = hMapBoxVerts_[dataOfs[i][j]];
			Vector sunDirection = (sunPos - pos).Normalize();
			float diffuseLightMult = 
				(((normal.dotP(sunDirection)) / 2.0f) + 0.5f);			
			Vector light = diffuse * diffuseLightMult + ambient;
			light[0] = MIN(1.0f, light[0]);
			light[1] = MIN(1.0f, light[1]);
			light[2] = MIN(1.0f, light[2]);

			float texX = hMapBoxVerts_[dataOfs[i][j]][0] / width;
			float texY = hMapBoxVerts_[dataOfs[i][j]][1] / height;

			glTexCoord2f(texX * texTileX / 4.0f, texY * texTileY / 4.0f);
			if (GLStateExtension::hasMultiTex())
			{
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, texX, texY);
				if (GLStateExtension::getTextureUnits() > 2)
				{
					glMultiTexCoord2fARB(GL_TEXTURE2_ARB, texX * texTileX, texY * texTileY);
				}
			}

			if (lightMap) glColor3fv(light);
			else glColor3f(1.0f, 1.0f, 1.0f);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3fv(pos);
		}
	}
	glEnd();
}
