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

#include <landscapemap/HeightMapLoader.h>
#include <image/ImageFactory.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <lang/LangResource.h>

void HeightMapLoader::loadTerrain(HeightMap &hmap, 
	Image &bitmap,
	bool levelSurround,
	ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("LOADING_LANDSCAPE", "Loading Landscape"));
	hmap.reset();

	fixed dhx = fixed(bitmap.getWidth()) / fixed(hmap.getMapWidth()+1);
	fixed dhy = fixed(bitmap.getHeight()) / fixed(hmap.getMapHeight()+1);

	int bh = bitmap.getHeight();
	int bw = bitmap.getWidth();

	fixed scale(true, 25000);

	fixed hy = fixed(0);
	for (int by=0; by<=hmap.getMapHeight(); by++, hy+=dhy)
	{
		if (counter) counter->setNewPercentage((100.0f * float(by)) / float(hmap.getMapHeight()));

		int ihy = hy.floor().asInt();
		int ihy2 = ihy + 1; if (ihy2 >= bh) --ihy2;

		int offsety  = ihy  * bw * 3;
		int offsety2 = ihy2 * bw * 3;
		unsigned char *posYA = (unsigned char*) (bitmap.getBits() + offsety);
		unsigned char *posYB = (unsigned char*) (bitmap.getBits() + offsety2);

		fixed hx = fixed(0);
		for (int bx=0; bx<=hmap.getMapWidth(); bx++, hx+=dhx)
		{
			int ihx = hx.floor().asInt();
			int ihx2 = ihx + 1; if (ihx2 >= bw) --ihx2;

			unsigned char *posXA1 = posYA + ihx * 3;
			unsigned char *posXA2 = posYA + ihx2 * 3;
			unsigned char *posXB1 = posYB + ihx * 3;
			unsigned char *posXB2 = posYB + ihx2 * 3;

			fixed heightXA1 = fixed(posXA1[0]);
			fixed heightXA2 = fixed(posXA2[0]);
			fixed heightXB1 = fixed(posXB1[0]);
			fixed heightXB2 = fixed(posXB2[0]);

			fixed XA = ((heightXA2 - heightXA1) * (hx - fixed(ihx))) + heightXA1;
			fixed XB = ((heightXB2 - heightXB1) * (hx - fixed(ihx))) + heightXB1;

			fixed h = ((XB - XA) * (hy - fixed(ihy))) + XA;

			hmap.setHeight(bx, by, h / scale);
		}
	}

	if (levelSurround) HeightMapModifier::levelSurround(hmap);
}

bool HeightMapLoader::generateTerrain(
	unsigned int seed,
	LandscapeDefnType *defn,
	HeightMap &hmap,
	bool &levelSurround,
	ProgressCounter *counter)
{
	// Do we generate or load the landscape
	if (defn->getType() == LandscapeDefnType::eHeightMapFile)
	{
		LandscapeDefnHeightMapFile *file = 
			(LandscapeDefnHeightMapFile *) defn;

		// Load the landscape
		levelSurround = file->levelsurround;
		
		Image image = ImageFactory::loadImage(S3D::eModLocation, file->file);
		if (!image.getBits())
		{
			S3D::dialogMessage("HeightMapLoader", S3D::formatStringBuffer(
				"Error: Unable to find landscape map \"%s\"",
				file->file.c_str()));
			return false;
		}
		if (!image.getLossless())
		{
			S3D::dialogExit("HeightMapLoader", S3D::formatStringBuffer(
				"Error: Deform landscape map \"%s\" is not a lossless image format",
				file->file.c_str()));
		}
		HeightMapLoader::loadTerrain(
			hmap,
			image, 
			file->levelsurround,
			counter);
	}
	else if (defn->getType() == LandscapeDefnType::eHeightMapGenerate)
	{
		LandscapeDefnHeightMapGenerate *generate = 
			(LandscapeDefnHeightMapGenerate *) defn;

		// Seed the generator and generate the landscape
		levelSurround = generate->levelsurround;
		FileRandomGenerator generator;
		FileRandomGenerator offsetGenerator;
		generator.seed(seed);
		offsetGenerator.seed(seed);

		HeightMapModifier::generateTerrain(
			hmap, 
			*generate, 
			generator, 
			offsetGenerator, 
			counter);
	}
	else 
	{
		S3D::dialogMessage("HeightMapLoader", S3D::formatStringBuffer(
			"Error: Unkown generate type %i",
			defn->getType()));
		return false;
	}

	// Make sure normals are correct for drawing
	if (counter) counter->setNewOp(LANG_RESOURCE("SETTINGS_NORMALS", "Setting Normals"));
	for (int y=0; y<hmap.getMapHeight(); y++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(y)) / float(hmap.getMapHeight()));

		for (int x=0; x<hmap.getMapWidth(); x++)
		{
			hmap.getNormal(x, y);
		}
	}
	
	return true;
}
