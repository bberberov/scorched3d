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

#if !defined(__INCLUDE_TargetVisibilityPatchh_INCLUDE__)
#define __INCLUDE_TargetVisibilityPatchh_INCLUDE__

#include <common/Vector.h>
#include <set>

class Target;
class TargetVisibilityPatch
{
public:
	TargetVisibilityPatch();
	~TargetVisibilityPatch();

	void setLocation(int x, int y, int w, int h);
	bool setVisible(float distance);
	void setNotVisible();

	bool hasTrees() { return !trees_.empty(); }
	bool hasTargets() { return !targets_.empty(); }

	float getDistance() { return distance_; }
	Vector &getPosition() { return position_; }
	bool getVisible() { return visible_; }

	static std::set<void *> &getLargeTargets() { return largeTargets_; }
	static void addLargeTarget(Target *target) { largeTargets_.insert(target); }
	static void removeLargeTarget(Target *target) { largeTargets_.erase(target); }

	std::set<void *> &getTargets() { return targets_; }
	void addTarget(Target *target) { targets_.insert(target); }
	void removeTarget(Target *target) { targets_.erase(target); }

	std::set<void *> &getTrees() { return trees_; }
	void addTree(Target *target) { trees_.insert(target); }
	void removeTree(Target *target) { trees_.erase(target); }	

	std::set<void *> &getTooltips() { return tooltips_; }
	void addTooltip(Target *target) { tooltips_.insert(target); }
	void removeTooltip(Target *target) { tooltips_.erase(target); }	

protected:
	int x_, y_;
	bool visible_;
	float distance_;
	Vector position_;
	std::set<void *> trees_, targets_, tooltips_;
	static std::set<void *> largeTargets_;
};

class TargetVisibilityIterator
{
public:
	void init(std::set<void *> &set)
	{
		set_ = &set;
		lastItor = set_->end();
		itor = set_->begin();
	}

	void *getNext()
	{
		if (itor == lastItor) return 0;
		void *result = *itor;
		++itor;
		return result;
	}

private:
	std::set<void *> *set_;
	std::set<void *>::iterator lastItor;
	std::set<void *>::iterator itor;
};

#endif // __INCLUDE_TargetVisibilityPatchh_INCLUDE__
