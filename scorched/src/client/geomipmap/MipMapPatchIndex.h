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

#if !defined(__INCLUDE_MipMapPatchIndexh_INCLUDE__)
#define __INCLUDE_MipMapPatchIndexh_INCLUDE__

class MipMapPatchIndex
{
public:
	MipMapPatchIndex();
	~MipMapPatchIndex();

	enum Borders
	{
		BorderLeft =   0x00000007,
		BorderRight =  0x00000038,
		BorderTop =    0x000001C0,
		BorderBottom = 0x00000E00
	};

	void generate(int size, int totalsize, int skip, unsigned int border, unsigned int totallods);
	void setBufferOffSet(int offset) { bufferOffSet_ = offset; }

	unsigned short *getIndices() { return indices_; }
	unsigned int getBufferOffSet() { return bufferOffSet_; }
	int getSize() { return size_; }
	int getMinIndex() { return minIndex_; }
	int getMaxIndex() { return maxIndex_; }

protected:
	int size_, minIndex_, maxIndex_;
	unsigned short *indices_;
	unsigned int bufferOffSet_;
};

#endif // __INCLUDE_MipMapPatchIndexh_INCLUDE__
