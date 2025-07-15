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


#if !defined(__INCLUDE_TankColorGeneratorh_INCLUDE__)
#define __INCLUDE_TankColorGeneratorh_INCLUDE__

#include <map>
#include <vector>
#include <common/Vector.h>

class Tank;
class TankColorGenerator
{
public:
	static TankColorGenerator *instance();

	Vector &getNextColor(std::map<unsigned int, Tank *> &tanks);

	bool colorAvailable(
		Vector &color, std::map<unsigned int, Tank *> &tanks,
		Tank *currentTank = 0);
	std::vector<Vector *> getAvailableColors(std::map<unsigned int, Tank *> &tanks,
		Tank *currentTank = 0);
	std::vector<Vector *> &getAllColors() { return availableColors_; }

	static Vector &getTeamColor(int team);
	static const char *getTeamName(int team);

protected:
	static TankColorGenerator *instance_;
	std::vector<Vector *> availableColors_;

	void addColor(unsigned r, unsigned g, unsigned b);

private:
	TankColorGenerator();
	virtual ~TankColorGenerator();
};

#endif
