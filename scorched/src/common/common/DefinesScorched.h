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

#ifndef __DEFINESSCORCHED__
#define __DEFINESSCORCHED__

namespace S3D
{
	void setExeName(const std::string &name);
	void setSettingsDir(const std::string &dir);
	void setDataFileMod(const std::string &mod);
	std::string getDataFileMod();
	std::string getExeName();
	std::string getStartTime();
	void showURL(const std::string &url);

	bool checkDataFile(const std::string &filename);
	std::string getDataFile(const std::string &filename);
	std::string getModFile(const std::string &filename);
	std::string getDocFile(const std::string &filename);
	std::string getLogFile(const std::string &filename);
	std::string getSettingsFile(const std::string &filename);
	std::string getHomeFile(const std::string &filename);
	std::string getSaveFile(const std::string &filename);
	std::string getSettingsModFile(const std::string &filename);
	std::string getGlobalModFile(const std::string &filename);

	extern unsigned int ScorchedPort;
	extern std::string ScorchedVersion;
	extern std::string ScorchedProtocolVersion;
	extern std::string ScorchedBuildTime;
}

#endif // __DEFINESSCORCHED__
