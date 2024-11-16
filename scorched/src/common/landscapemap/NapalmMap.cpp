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

#include <landscapemap/NapalmMap.h>
#include <common/DefinesAssert.h>
#include <memory.h>

NapalmMap::NapalmMap() : entries_(0)
{
}

NapalmMap::~NapalmMap()
{
	delete [] entries_;
	entries_ = 0;
}

fixed &NapalmMap::getNapalmHeight(int w, int h) 
{
	DIALOG_ASSERT(entries_);

	if (w >= 0 && h >= 0 && w<=width_ && h<=height_)
	{
        return entries_[(width_+1) * h + w]; 
	}

	static fixed tmp(1000);
	tmp = fixed(1000);
	return tmp;
}

void NapalmMap::create(int width, int height)
{
	width_ = width;
    height_ = height;
	delete [] entries_;

	entries_ = new fixed[(width_ + 1) * (height_ + 1)];
	clear();
}

void NapalmMap::clear()
{
	if (entries_)
	{
		memset(entries_, 0, sizeof(float) * (width_ + 1) * (height_ + 1));
	}
}
