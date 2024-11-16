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

#if !defined(__INCLUDE_TankViewPointsh_INCLUDE__)
#define __INCLUDE_TankViewPointsh_INCLUDE__

#include <set>
#include <common/FixedVector.h>

class TankViewPointProvider
{
public:
	static FixedVector defaultLookFrom;

	TankViewPointProvider() : referenceCount_(0)
	{
	}
	void incrementReference() { referenceCount_++; }
	void decrementReference() { if (--referenceCount_ == 0) delete this; }
	int getReferenceCount() { return referenceCount_; }

	void setValues(const FixedVector position, 
		const FixedVector &lookFrom = defaultLookFrom,
		const fixed radius = 1) 
	{
		lookFrom_ = lookFrom;
		position_ = position;
		radius_ = radius;
	}

	FixedVector &getLookFrom() { return lookFrom_; }
	FixedVector &getPosition() { return position_; }
	fixed getRadius() { return radius_; }

protected:
	~TankViewPointProvider() {}

	FixedVector lookFrom_, position_;
	fixed radius_;
	int referenceCount_;
};

class ScorchedContext;
class TankViewPoints
{
public:
	TankViewPoints(ScorchedContext &context);
	virtual ~TankViewPoints();

	void addViewPoint(TankViewPointProvider *provider);
	void removeViewPoint(TankViewPointProvider *provider);
	bool hasViewPoints() { return !points_.empty(); }

	bool getValues(FixedVector &lookAt, FixedVector &lookFrom);

protected:
	std::set<TankViewPointProvider *> points_;
	ScorchedContext &context_;
};

class TankViewPointsCollection
{
public:
	TankViewPointsCollection(ScorchedContext &context);

	TankViewPoints &getExplosionViewPoints() 
		{ return explosionViewPoints_; }
	TankViewPoints &getProjectileViewPoints() 
		{ return projectileViewPonts_; }

	static std::set<unsigned int> TankViewPointsTanks;

protected:
	TankViewPoints explosionViewPoints_;
	TankViewPoints projectileViewPonts_;
};

#endif
