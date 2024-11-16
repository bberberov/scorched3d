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

#if !defined(AFX_ImageMODIFIER_H__3C6E8BBF_1AB2_4847_BEB1_934C97C76F05__INCLUDED_)
#define AFX_ImageMODIFIER_H__3C6E8BBF_1AB2_4847_BEB1_934C97C76F05__INCLUDED_

#include <image/Image.h>
#include <landscapemap/HeightMap.h>
#include <common/Vector.h>

class ScorchedContext;
namespace ImageModifier  
{
	bool findIntersection(HeightMap &hMap,
							Vector start,
							Vector end,
							float &dist,
							float stopDist);

	void tileBitmap(Image &src, Image &dest);

	void addLightMapToBitmap(Image &bitmap, 
							HeightMap &hMap,
							Vector &sunPos,
							Vector &ambience,
							Vector &diffuse,
							ProgressCounter *counter = 0);

	void addHeightToBitmap(HeightMap &hMap,
							Image &destBitmap, 
							Image &slopeBitmap,
							Image &shoreBitmap,
							Image **heightBitmaps,
							int numberSources,
							int destBitmapScaleSize,
							ProgressCounter *counter = 0);

	void redBitmap(
		Image &destBitmap);

	void addTexturesToBitmap(
		Image &destBitmap,
		Image &slopeBitmap,
		Image &shoreBitmap,
		Image **heightBitmaps,
		int numberSources);


	void addWaterToBitmap(HeightMap &hMap,
							Image &destBitmap,
							Image &waterBitmap,
							float waterHeight);

	void removeWaterFromBitmap(HeightMap &hMap,
							Image &srcBitmap,
							Image &destBitmap,
							Image &alphaBitmap,
							float waterHeight);

	Image makeArenaBitmap();
	Image makeArenaSurroundBitmap();

	void addBorderToBitmap(Image &destBitmap,
		int borderWidth,
		float colors[3]);

	void makeBitmapTransparent(Image &output,
		Image &input,
		Image &mask);

	void addCircleToLandscape(
		ScorchedContext &context,
		float sx, float sy, float sw, float opacity);
	void addCircle(Image &destBitmap,
		float sx, float sy, float sw, float opacity);

	void addBitmapToLandscape(
		ScorchedContext &context,
		Image &srcBitmap,
		float sx, float sy, float scalex, float scaley, 
		bool commit = false);
	void addBitmap(Image &destBitmap,
		Image &srcBitmap,
		float sx, float sy, float scalex, float scaley, 
		bool commit);

	void scalePlanBitmap(Image &destBitmap,
		Image &srcBitmap,
		int landscapeX, int landscapeY);
};

#endif // !defined(AFX_ImageMODIFIER_H__3C6E8BBF_1AB2_4847_BEB1_934C97C76F05__INCLUDED_)
