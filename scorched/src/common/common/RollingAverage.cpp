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

#include <common/RollingAverage.h>

RollingAverage::RollingAverage(int numberAverages, fixed startValue) :
	numberAverages_(numberAverages)
{
	averages_ = new fixed[numberAverages_];
	reset(startValue);
}

RollingAverage::~RollingAverage()
{
	delete [] averages_;
}

void RollingAverage::reset(fixed value)
{
	average_ = value;
	index_ = 0;
	total_ = 0;
	for (int i=0; i<numberAverages_; i++)
	{
		averages_[i] = value;
		total_ += value;
	}
}

void RollingAverage::addValue(fixed value)
{
	index_ = (++index_ % numberAverages_);

	total_ -= averages_[index_];
	averages_[index_] = value;
	total_ += value;

	average_ = total_ / numberAverages_;
}
