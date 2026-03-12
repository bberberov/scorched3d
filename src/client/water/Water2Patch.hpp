////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_Water2Patch_hpp_INCLUDE__
#define __INCLUDE_Water2Patch_hpp_INCLUDE__

#include <geomipmap/MipMapPatchIndexs.hpp>
#include <water/Water2Constants.hpp>
#include <vector>

class Water2Patch
{
public:
	Water2Patch();
	~Water2Patch();

	struct Data
	{
		// FIXME, we don't need all this info all the time!
		float x, y, z;
		float nx, ny, nz;
	};

	// clang-format off
	// uncrustify off
	void generate( Water2Points& heights, int size, int totalSize, int posX, int posY, float waterHeight );
	void setBufferOffSet( int offset ) { bufferOffSet_ = offset; }
	int  getBufferOffSet()             { return bufferOffSet_; }

	void  draw( MipMapPatchIndex& index );
	Data* getData( int x, int y );
	Data* getInternalData() { return data_; }
	// uncrustify on
	// clang-format on

protected:
	Data* data_;
	int   dataSize_;
	int   size_;
	int   bufferOffSet_;
};

#endif  // __INCLUDE_Water2Patch_hpp_INCLUDE__
