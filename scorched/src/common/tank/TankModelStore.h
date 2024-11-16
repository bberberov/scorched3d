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

#ifndef _tankModelStore_h
#define _tankModelStore_h

#include <vector>
#include <map>
#include <set>
#include <tank/TankModel.h>
#include <common/ProgressCounter.h>

class TankModelStore
{
public:
	TankModelStore();
	virtual ~TankModelStore();

	bool loadTankMeshes(ScorchedContext &context, int detailLevel,
		ProgressCounter *counter = 0);
	void clear();

	TankModel *getModelByName(const char *name);
	TankModel *getRandomModel(int team, bool ai, const char *tankType);

	std::vector<TankModel *> &getModels() { return models_; }
	std::set<std::string> &getModelCatagories() { return modelCatagories_; }

protected:
	std::vector<TankModel *> models_;
	std::set<std::string, std::less<std::string> > modelCatagories_;

	void addModels(std::vector<TankModel *> &src);
	void killModels(std::vector<TankModel *> &src);

private:
	TankModelStore(const TankModelStore &);
	const TankModelStore & operator=(const TankModelStore &);

};

#endif // _tankModelStore_h

