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

#include <landscape/InfoMap.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <image/ImageFactory.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <common/OptionsTransient.h>

InfoMap *InfoMap::instance_ = 0;

InfoMap *InfoMap::instance()
{
	if (!instance_)
	{
		instance_ = new InfoMap;
	}
	return instance_;
}

InfoMap::InfoMap()
{
}

InfoMap::~InfoMap()
{
}

void InfoMap::addAdapters()
{
	static ConsoleRuleMethodIAdapter<Landscape> *off = 0;
	static ConsoleRuleMethodIAdapter<InfoMap> *bands = 0;
	static ConsoleRuleMethodIAdapter<InfoMap> *grid = 0;
	
	delete off; off = 0;
	delete bands; bands = 0;
	delete grid; grid = 0;

	if(ScorchedClient::instance()->getOptionsGame().getDebugFeatures() ||
		!ClientParams::instance()->getConnectedToServer())
	{
		off = new ConsoleRuleMethodIAdapter<Landscape>(
			Landscape::instance(), &Landscape::restoreLandscapeTexture, "LandscapeInfoOff");
		bands = new ConsoleRuleMethodIAdapter<InfoMap>(
			this, &InfoMap::showHeightBands, "LandscapeInfoHeightBands");
		grid = new ConsoleRuleMethodIAdapter<InfoMap>(
			this, &InfoMap::showGrid, "LandscapeInfoGrid");
	}
}

void InfoMap::showHeightBands()
{
	Image newMap(
		Landscape::instance()->getMainMap().getWidth(),
		Landscape::instance()->getMainMap().getHeight());
	float *heights = new float[
		Landscape::instance()->getMainMap().getWidth() *
		Landscape::instance()->getMainMap().getHeight()];

	const float width = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	const float height = (float)
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight();
	const float sqSizeWidth = width / float(newMap.getWidth());
	const float sqSizeHeight = height / float(newMap.getHeight());
	const float heightSep = 3.0f;
	
	int y;
	for (y=0; y<newMap.getHeight(); y++)
	{
		float posY = float(y) * sqSizeHeight;
		for (int x=0; x<newMap.getWidth(); x++)
		{
			float posX = float(x) * sqSizeWidth;
			heights[x + y * newMap.getWidth()] = 
				ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getInterpHeight(
						fixed::fromFloat(posX), fixed::fromFloat(posY)).asFloat();
		}
	}

	GLubyte *dest = newMap.getBits();
	GLubyte *src = Landscape::instance()->getMainMap().getBits();
	for (y=0; y<newMap.getHeight(); y++)
	{
		for (int x=0; x<newMap.getWidth(); x++)
		{
			GLubyte r = src[0];
			GLubyte g = src[1];
			GLubyte b = src[2];

			if (x!=0 && y!=0 && x<newMap.getWidth()-1 && y<newMap.getHeight()-1)
			{
			float height = heights[x + y * newMap.getWidth()];
			float height2 = heights[x + 1 + y * newMap.getWidth()];
			float height3 = heights[x - 1 + y * newMap.getWidth()];
			float height4 = heights[x + (y +1) * newMap.getWidth()];
			float height5 = heights[x + (y -1) * newMap.getWidth()];
			float baseHeight = float(int(height / heightSep)) * heightSep;
			float baseHeight2 = float(int(height2 / heightSep)) * heightSep;
			float baseHeight3 = float(int(height3 / heightSep)) * heightSep;
			float baseHeight4 = float(int(height4 / heightSep)) * heightSep;
			float baseHeight5 = float(int(height5 / heightSep)) * heightSep;

			if (baseHeight < baseHeight2 || baseHeight < baseHeight3 ||
				baseHeight < baseHeight4 || baseHeight < baseHeight5)
			{
				r = g = b = 0;
			}
			else if (baseHeight > baseHeight2 || baseHeight > baseHeight3 ||
                        	baseHeight > baseHeight4 || baseHeight > baseHeight5)
			{
       				 r = g = b = 255;
			}
			}

			dest[0] = r;
			dest[1] = g;
			dest[2] = b;

			dest+=3;
			src+=3;
		}
	}

	Landscape::instance()->getMainTexture().replace(newMap, false);
	Landscape::instance()->setTextureType(Landscape::eOther);

	delete [] heights;
}

void InfoMap::showGrid()
{
	Image newMap(
		Landscape::instance()->getMainMap().getWidth(),
		Landscape::instance()->getMainMap().getHeight());

	GLubyte *dest = newMap.getBits();
	GLubyte *src = Landscape::instance()->getMainMap().getBits();
	for (int y=0; y<newMap.getHeight(); y++)
	{
		for (int x=0; x<newMap.getWidth(); x++)
		{
			GLubyte r = src[0];
			GLubyte g = src[1];
			GLubyte b = src[2];

			if (x % 15 == 0 || y % 15 == 0)
			{
				r = g = b = 255;
			}
			else if ((1+x) % 15 == 0 || (1+y) % 15 == 0)
			{
                        	r = g = b = 0;
			}
	
			dest[0] = r;
			dest[1] = g;
			dest[2] = b;

			dest+=3;
			src+=3;
		}
	}

	Landscape::instance()->getMainTexture().replace(newMap, false);
	Landscape::instance()->setTextureType(Landscape::eOther);
}
