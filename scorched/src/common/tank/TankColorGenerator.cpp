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

#include <tank/TankColorGenerator.h>
#include <tank/Tank.h>
#include <stdlib.h>

TankColorGenerator *TankColorGenerator::instance_ = 0;

TankColorGenerator *TankColorGenerator::instance()
{
	if (!instance_)
	{
		instance_ = new TankColorGenerator;
	}
	return instance_;
}

TankColorGenerator::TankColorGenerator()
{
	addColor(255, 0, 0);
	addColor(255, 255, 0);
	addColor(255, 120, 0);
	addColor(140, 0, 255);
	addColor(255, 0, 255);
	addColor(0, 0, 255);
	addColor(0, 255, 0);
	addColor(255, 190, 0);
	addColor(255, 255, 140);
	addColor(140, 255, 255);
	addColor(140, 140, 255);
	addColor(140, 0, 0);
	addColor(140, 140, 0);
	addColor(140, 140, 140);
	addColor(140, 255, 0);
	addColor(255, 0, 140);
	addColor(140, 255, 140);	
	addColor(0, 0, 140);
	addColor(0, 140, 0);
	addColor(0, 255, 140);
	addColor(140, 0, 140);
	addColor(0, 255, 255);
	addColor(0, 140, 255);
	addColor(0, 140, 140);
	addColor(255, 140, 255);
	addColor(190, 125, 35);
}

TankColorGenerator::~TankColorGenerator()
{
}

void TankColorGenerator::addColor(unsigned r, unsigned g, unsigned b)
{
	Vector newColor(
		float(int(float(r) / 2.550f)) / 100.0f, 
		float(int(float(g) / 2.550f)) / 100.0f, 
		float(int(float(b) / 2.550f)) / 100.0f);
	availableColors_.push_back(new Vector(newColor));
}

bool TankColorGenerator::colorAvailable(Vector &color,
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	bool available = true;
	std::map<unsigned int, Tank *>::iterator tankitor;
	for (tankitor = tanks.begin();
		tankitor != tanks.end();
		++tankitor)
	{
		Tank *tank = (*tankitor).second;
		if (tank->getColor() == color)
		{
			if (currentTank != tank)
			{
				available = false;
				break;
			}
		}
	}

	return available;
}

std::vector<Vector *> TankColorGenerator::getAvailableColors(
	std::map<unsigned int, Tank *> &tanks,
	Tank *currentTank)
{
	std::vector<Vector*> leftColors;
	std::vector<Vector*>::iterator coloritor;
	for (coloritor = availableColors_.begin();
		coloritor != availableColors_.end();
		++coloritor)
	{
		Vector *color = (*coloritor);
		if (colorAvailable(*color, tanks, currentTank))
		{
			leftColors.push_back(color);
		}
	}
	return leftColors;
}

Vector &TankColorGenerator::getNextColor(std::map<unsigned int, Tank *> &tanks)
{
	std::vector<Vector *> leftColors = getAvailableColors(tanks);
	if (!leftColors.empty())
	{
		return *leftColors[0];
	}

	static Vector defaultColor(0.8f, 0.8f, 0.8f);
	return defaultColor;
}

Vector &TankColorGenerator::getTeamColor(int team)
{
	static Vector red(1.0f, 0.0f, 0.0f);
	static Vector blue(0.0f, 0.3f, 1.0f);
	static Vector green(0.0f, 1.0f, 0.0f);
	static Vector yellow(1.0f, 1.0f, 0.0f);
	if (team == 1) return red;
	else if (team == 2) return blue;
	else if (team == 3) return green;
	else if (team == 4) return yellow;
	return Vector::getNullVector();
}

const char *TankColorGenerator::getTeamName(int team)
{
	if (team == 1) return "Red";
	else if (team == 2) return "Blue";
	else if (team == 3) return "Green";
	else if (team == 4) return "Yellow";
	return "None";
}
