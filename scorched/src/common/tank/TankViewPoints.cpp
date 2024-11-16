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

#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <tank/TankViewPoints.h>
#include <target/TargetContainer.h>

std::set<unsigned int> TankViewPointsCollection::TankViewPointsTanks;

FixedVector TankViewPointProvider::defaultLookFrom(5, 5, 25);

TankViewPoints::TankViewPoints(ScorchedContext &context) : 
	context_(context)
{
}

TankViewPoints::~TankViewPoints()
{
}

bool TankViewPoints::getValues(FixedVector &lookAt, FixedVector &lookFrom)
{
	FixedVector 
		max = FixedVector::getMaxVector(), 
		min = FixedVector::getMinVector();
	fixed count = 0;

	lookAt.zero();
	lookFrom.zero();

	std::set<TankViewPointProvider *>::iterator itor =
		points_.begin();
	std::set<TankViewPointProvider *>::iterator enditor =
		points_.end();
	for (; itor != enditor; ++itor)
	{
		TankViewPointProvider *provider = *itor;

		FixedVector &itorPosition = provider->getPosition();
		FixedVector &itorLookAt = provider->getLookFrom();
		fixed itorRadius = provider->getRadius();

		min[0] = MIN(min[0], itorPosition[0] - itorRadius);
		min[1] = MIN(min[1], itorPosition[1] - itorRadius);
		min[2] = MIN(min[2], itorPosition[2] - itorRadius);
		max[0] = MAX(max[0], itorPosition[0] + itorRadius);
		max[1] = MAX(max[1], itorPosition[1] + itorRadius);
		max[2] = MAX(max[2], itorPosition[2] + itorRadius);		

		lookAt += itorPosition;
		lookFrom += itorLookAt;
		count += 1;
	}

	fixed dist = 25;
	fixed maxMin = (max - min).Magnitude();
	if (maxMin > 0)
	{
		dist = maxMin + 25;
	}
	lookFrom.StoreNormalize();

	lookAt /= count;
	lookFrom *= dist;

	return !points_.empty();
}

void TankViewPoints::addViewPoint(TankViewPointProvider *provider)
{
	points_.insert(provider);
}
	
void TankViewPoints::removeViewPoint(TankViewPointProvider *provider)
{
	points_.erase(provider);
}

TankViewPointsCollection::TankViewPointsCollection(ScorchedContext &context) :
	explosionViewPoints_(context),
	projectileViewPonts_(context)
{
}