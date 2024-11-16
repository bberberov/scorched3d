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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <common/DefinesScorched.h>
#include <common/DefinesFile.h>
#include <common/DefinesString.h>
#include <common/DefinesAssert.h>
#include <common/Logger.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(disable : 4996)

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

unsigned int S3D::ScorchedPort = 27270;
std::string S3D::ScorchedVersion = "44"
#ifdef _DEBUG
	" **DEBUG**"
#endif
;
std::string S3D::ScorchedProtocolVersion = "ew";
#ifdef __DATE__
std::string S3D::ScorchedBuildTime = __DATE__;
#else
std::string S3D::ScorchedBuildTime = "Unknown";
#endif
static std::string exeName;
static std::string currentMod = "none";
static std::string settingsDir = "";

void S3D::showURL(const std::string &url)
{
#ifdef _WIN32
	std::string buffer = S3D::formatStringBuffer("explorer %s", url.c_str());
	WinExec(buffer.c_str() ,SW_SHOWDEFAULT);
#else
#ifdef __DARWIN__
	std::string buffer = S3D::formatStringBuffer("open %s", url.c_str());
	system(buffer.c_str());
#else
	std::string buffer = S3D::formatStringBuffer("firefox %s", url.c_str());
	system(buffer.c_str());
#endif // __DARWIN__
#endif // _WIN32
	Logger::log(url);
}

void S3D::setExeName(const std::string &name)
{
	exeName = name;
}

std::string S3D::getExeName()
{
	return exeName;
}

std::string S3D::getStartTime()
{
	static std::string startTime;
	if (startTime.empty())
	{
		time_t theTime = time(0);
		startTime = ctime(&theTime);
	}
	return startTime;
}

void S3D::setSettingsDir(const std::string &dir)
{
	settingsDir = dir;

	if (S3D::stristr(S3D::ScorchedVersion.c_str(), "BETA"))
	{
		settingsDir.append("BETA");
	}
}

void S3D::setDataFileMod(const std::string &mod)
{
	currentMod = mod;
}

std::string S3D::getDataFileMod()
{
	return currentMod;
}

#ifndef S3D_DATADIR
#define S3D_DATADIR "."
#endif
#ifndef S3D_DOCDIR
#define S3D_DOCDIR "./documentation"
#endif
#ifndef S3D_BINDIR
#define S3D_BINDIR "."
#endif

static const char *GET_DIR(const char *dir)
{
	if (dir[0] == '.')
	{
		static char path[1024];
#ifdef _WIN32
			GetCurrentDirectory(sizeof(path), path);
#else
			getcwd(path, sizeof(path));
#endif // _WIN32
		if (strlen(path) + strlen(dir) + 1 < sizeof(path))
		{
			strcat(path, "/");
			strcat(path, dir);
		}
		return path;
	}
	return dir;
}

std::string S3D::getModFile(const std::string &filename)
{
	std::string buffer;

	buffer = S3D::getSettingsModFile(
		S3D::formatStringBuffer("%s/%s", currentMod.c_str(), filename.c_str()));
	S3D::fileDos2Unix(buffer);
	if (S3D::fileExists(buffer)) return buffer;

	buffer = S3D::getGlobalModFile(
		S3D::formatStringBuffer("%s/%s", currentMod.c_str(), filename.c_str()));
	S3D::fileDos2Unix(buffer);
	if (S3D::fileExists(buffer)) return buffer;

	if (currentMod != "none")
	{
		buffer = S3D::getGlobalModFile(
			S3D::formatStringBuffer("%s/%s", "none", filename.c_str()));
		S3D::fileDos2Unix(buffer);
		if (S3D::fileExists(buffer)) return buffer;
	}

	return buffer;
}

std::string S3D::getDataFile(const std::string &filename)
{
	std::string buffer = S3D::formatStringBuffer("%s/%s", GET_DIR(S3D_DATADIR), filename.c_str());
	S3D::fileDos2Unix(buffer);

	return buffer;
}

extern bool S3D::checkDataFile(const std::string &filename)
{
	std::string dataFileName = S3D::getModFile(filename);
	if (!S3D::fileExists(dataFileName))
	{
		if (0 == strstr(filename.c_str(), "none"))
		{
			S3D::dialogMessage("Scorched3D", S3D::formatStringBuffer(
				"The file \"%s\" does not exist",
				dataFileName.c_str()));
			return false;
		}
	}
	return true;
}

std::string S3D::getDocFile(const std::string &filename)
{
	std::string buffer =
		S3D::formatStringBuffer("%s/%s", GET_DIR(S3D_DOCDIR), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getHomeFile(const std::string &filename)
{
	static std::string homeDir;
	if (!homeDir.c_str()[0])
	{
		homeDir = GET_DIR(S3D_DATADIR);
		if (S3D::dirExists(S3D::getHomeDir()))
		{
			homeDir = S3D::getHomeDir();
		}
	}

	std::string buffer = S3D::formatStringBuffer(
		"%s/%s", homeDir.c_str(), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getSettingsFile(const std::string &filename)
{
	static std::string homeDir;
	if (!homeDir.c_str()[0])
	{
		DIALOG_ASSERT(settingsDir.c_str() && settingsDir.c_str()[0]);

		std::string homeDirStr = S3D::getHomeFile(
			S3D::formatStringBuffer("/%s", settingsDir.c_str()));
		if (!S3D::dirExists(homeDirStr))
		{
			if (!S3D::dirMake(homeDirStr))
			{
				homeDirStr = S3D::getHomeFile("");
			}
		}
		homeDir = homeDirStr;
	}

	std::string buffer = S3D::formatStringBuffer(
		"%s/%s", homeDir.c_str(), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getLogFile(const std::string &filename)
{
	std::string homeDirStr = S3D::getSettingsFile("");
	std::string newDir(std::string(homeDirStr) + std::string("/logs"));
	if (S3D::dirExists(newDir)) homeDirStr = newDir;
	else if (S3D::dirMake(newDir)) homeDirStr = newDir;

	std::string buffer = S3D::formatStringBuffer(
		"%s/%s", homeDirStr.c_str(), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getSaveFile(const std::string &filename)
{
	std::string homeDirStr = S3D::getSettingsFile("");
	std::string newDir(std::string(homeDirStr) + std::string("/saves"));
	if (S3D::dirExists(newDir)) homeDirStr = newDir;
	else if (S3D::dirMake(newDir)) homeDirStr = newDir;

	std::string buffer = S3D::formatStringBuffer(
		"%s/%s", homeDirStr.c_str(), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getSettingsModFile(const std::string &filename)
{
	static std::string modDir;
	if (!modDir.c_str()[0])
	{
		std::string homeDirStr = S3D::getSettingsFile("");
		std::string newDir(std::string(homeDirStr) + std::string("/mods"));
		if (S3D::dirExists(newDir)) homeDirStr = newDir;
		else if (S3D::dirMake(newDir)) homeDirStr = newDir;

		modDir = homeDirStr;
	}
	         
	std::string buffer = S3D::formatStringBuffer(
		"%s/%s", modDir.c_str(), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

std::string S3D::getGlobalModFile(const std::string &filename)
{
	std::string buffer = S3D::formatStringBuffer(
		"%s/data/globalmods/%s", GET_DIR(S3D_DATADIR), filename.c_str());
	S3D::fileDos2Unix(buffer);
	return buffer;
}

