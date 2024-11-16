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

#if !defined(__INCLUDE_Water2Patchh_INCLUDE__)
#define __INCLUDE_Water2Patchh_INCLUDE__

#include <geomipmap/MipMapPatchIndexs.h>
#include <water/Water2Constants.h>
#include <vector>

class Water2Patch
{
public:
	Water2Patch();
	~Water2Patch();

	struct Data
	{
		// Fix me, we don't need all this info all the time!
		float x, y, z;
		float nx, ny, nz;
	};

	void generate(Water2Points &heights,
		int size, int totalSize,
		int posX, int posY,
		float waterHeight);
	void setBufferOffSet(int offset) { bufferOffSet_ = offset; }
	int getBufferOffSet() { return bufferOffSet_; }

	void draw(MipMapPatchIndex &index);
	Data *getData(int x, int y);
	Data *getInternalData() { return data_; }

protected:
	int size_, dataSize_;
	int bufferOffSet_;
	Data *data_;	
};

#endif // __INCLUDE_Water2Patchh_INCLUDE__
