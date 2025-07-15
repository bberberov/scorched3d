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

#include <landscape/DeformTextures.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLImageModifier.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Defines.h>
#include <math.h>

void DeformTextures::deformLandscape(Vector &pos, float radius, 
	Image &scorchedMap, 
	DeformLandscape::DeformPoints &map)
{
	DIALOG_ASSERT(Landscape::instance()->getMainMap().getComponents() == 3);

	HeightMap &hmap = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();
	int iradius = (int) radius + 1;
	if (iradius > 49) iradius = 49;
	int idiam = iradius * 2;

	float pixelsPerSW = (float)(Landscape::instance()->getMainMap().getWidth()) / float(hmap.getMapWidth());
	float pixelsPerSH = (float)(Landscape::instance()->getMainMap().getHeight()) / float(hmap.getMapHeight());

	GLint x = GLint((pos[0] - iradius - 1) * pixelsPerSW);
	GLint y = GLint((pos[1] - iradius - 1) * pixelsPerSH);
	GLsizei w = GLsizei(pixelsPerSW * 2.0f * iradius);
	GLsizei h = GLsizei(pixelsPerSH * 2.0f * iradius);

	x = MAX(x, 0);
	y = MAX(y, 0);
	w = MIN(w, Landscape::instance()->getMainMap().getWidth() - x);
	h = MIN(h, Landscape::instance()->getMainMap().getHeight() - y);

	if (!GLStateExtension::getNoTexSubImage() &&
		x < Landscape::instance()->getMainMap().getWidth() &&
		y < Landscape::instance()->getMainMap().getHeight() && 
		x + w < Landscape::instance()->getMainMap().getWidth() &&
		y + h < Landscape::instance()->getMainMap().getHeight())
	{
		int landscapeWidth = Landscape::instance()->getMainMap().getWidth();
		int width = 3 * landscapeWidth;
		width   = (width + 3) & ~3;	
		int destWidthInc = width - w * 3;

		float posX = 0, posY = 0;
		float posXinc = 1.0f / pixelsPerSW;
		float posYinc = 1.0f / pixelsPerSH;

		GLubyte *bytes = 
			Landscape::instance()->getMainMap().getBits() + ((width * y) + x * 3);
		GLubyte *destBits = bytes;
		for (int b=0; b<h;b++, posY+=posYinc)
		{
            int mapY1 = int(posY);
			int mapY2 = mapY1 + 1;
			if (mapY2 >= idiam) mapY2 = mapY1;

			posX = 0.0f;
			for (int a=0; a<w; a++, posX+=posXinc)
			{
				int mapX1 = int(posX);
				int mapX2 = mapX1 + 1;
				if (mapX2 >= idiam) mapX2 = mapY1;

				if (mapX1 < idiam && mapY1 < idiam) 
				{
					float magx1y1 = map.map[mapX1][mapY1].asFloat();
					float magx2y1 = map.map[mapX2][mapY1].asFloat();
					float magx1y2 = map.map[mapX1][mapY2].asFloat();
					float magx2y2 = map.map[mapX2][mapY2].asFloat();

					float dx = posX - float(mapX1);
					float dy = posY - float(mapY1);

					float ymag1 = (magx1y2 * dy) + (magx1y1 * (1.0f - dy));
					float ymag2 = (magx2y2 * dy) + (magx2y1 * (1.0f - dy));

					float mag = (ymag2 * dx) + (ymag1 * (1.0f - dx));
					if (mag > 0.0f)
					{
						int posX = (x + a) % scorchedMap.getWidth();
						int posY = (y + b) % scorchedMap.getHeight();
						GLubyte *srcBits = 
							scorchedMap.getBits() + ((scorchedMap.getWidth() * posY * 
							scorchedMap.getComponents()) + (posX * scorchedMap.getComponents()));

						if (scorchedMap.getComponents() == 1)
						{
							destBits[0] = (GLubyte) ((float(srcBits[0]) * mag) + 
								(float(destBits[0]) * (1.0f - mag)));
							destBits[1] = (GLubyte) ((float(srcBits[0]) * mag) + 
								(float(destBits[1]) * (1.0f - mag)));
							destBits[2] = (GLubyte) ((float(srcBits[0]) * mag) + 
								(float(destBits[2]) * (1.0f - mag)));
						}
						else
						{
							destBits[0] = (GLubyte) ((float(srcBits[0]) * mag) + 
								(float(destBits[0]) * (1.0f - mag)));
							destBits[1] = (GLubyte) ((float(srcBits[1]) * mag) + 
								(float(destBits[1]) * (1.0f - mag)));
							destBits[2] = (GLubyte) ((float(srcBits[2]) * mag) + 
								(float(destBits[2]) * (1.0f - mag)));
						}
					}
				}
				destBits +=3;
			}
			destBits += destWidthInc;
		}

		GLState currentState(GLState::TEXTURE_ON);
		Landscape::instance()->getMainTexture().draw(true);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, landscapeWidth);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 
						x, y, 
						w, h, 
						GL_RGB, GL_UNSIGNED_BYTE, 
						bytes);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
}
