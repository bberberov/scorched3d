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

#include <engine/ModDirs.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/FileList.h>
#include <stdio.h>

ModDirs::ModDirs()
{
}

ModDirs::~ModDirs()
{
}

bool ModDirs::loadModDirs()
{
	loadModFile("none", true);

	std::string modFile = S3D::getSettingsModFile("");
	std::string globalModFile = S3D::getGlobalModFile("");
	if (!loadModDir(modFile, false)) return false;
	if (!loadModDir(globalModFile, true)) return false;
	return true;
}
	
bool ModDirs::loadModDir(const std::string &dirName, bool global)
{
	FileList dir(dirName.c_str(), "*", false);
	if (dir.getStatus())
	{
		FileList::ListType &dirs = dir.getFiles();
		FileList::ListType::iterator itor;
		for (itor = dirs.begin();
			itor != dirs.end();
			++itor)
		{
			std::string filename = (*itor);
			if (!loadModFile(filename, global)) return false;
		}
	}
	return true;
}

bool ModDirs::loadModFile(const std::string &inputFileName, bool global)
{
	if (strstr(inputFileName.c_str(), "CVS")) return true;

	std::string oldFileName(inputFileName);
	std::string newFileName(inputFileName);
	_strlwr((char *) newFileName.c_str());

	if (oldFileName != newFileName)
	{
		S3D::dialogMessage("ModDirs", S3D::formatStringBuffer(
			"ERROR: All mod directories must have lower case filenames.\n"
			"Directory \"%s\" has upper case charaters in it",
			newFileName.c_str()));
		return false;
	}
	
	// Check if this mod has already been loaded
	std::list<ModInfo>::iterator itor;
	for (itor = dirs_.begin();
		itor != dirs_.end();
		++itor)
	{
		ModInfo &info = (*itor);
		if (0 == strcmp(info.getName(), newFileName.c_str())) return true;
	}

	// Check if there is a modinfo file for this mod
	std::string noneGamesFile = S3D::getModFile("data/modinfo.xml");
	S3D::setDataFileMod(newFileName);
	std::string modGamesFile = S3D::getModFile("data/modinfo.xml");
	S3D::setDataFileMod("none");

	if (!S3D::fileExists(modGamesFile))
	{
		// This mod directory does not exist
		return true;
	}

	bool defaultInfoFile = (noneGamesFile == modGamesFile);
	if (newFileName != "none" && defaultInfoFile)
	{
		// This mod does not have a mod info, no one can play it
		// or even see it
		return true;
	}

	ModInfo newInfo(newFileName.c_str());

	S3D::setDataFileMod(newFileName);
	bool parseResult = newInfo.parse(modGamesFile.c_str());
	S3D::setDataFileMod("none");

	if (!parseResult)
	{
		// Just log and return as this may be the result of
		// only downloading 1/2 the mod and not having the icons required
		// for the modinfo
		Logger::log(S3D::formatStringBuffer(
			"Failed to parse mod info file \"%s\"",
			modGamesFile.c_str()));
		return true;
	}
	

	if (0 != strcmp(newInfo.getProtocolVersion(), S3D::ScorchedProtocolVersion.c_str()))
	{
		// Check if this is a global mod (one bundled with s3d)
		// If its not we can't play it as it is out of date
		// If it is we remove the custom version in the home dir
		if (!global)
		{
			return true;
		}

		// The version has changed move the current mod directories
		if (!S3D::dirExists(S3D::getSettingsFile("/oldmods")))
		{
			S3D::dirMake(S3D::getSettingsFile("/oldmods"));
		}

		// The gloabl mod is out of date, move it out the way
		// This is done because this mod is included with S3D (i.e. a global mod)
		// but also exists in the users custom mod directory.
		// The one in the user's directory may be out of date, so it needs to be 
		// removed incase it masks the newer one that comes with the game.
		std::string src = S3D::getSettingsModFile(newFileName);
		std::string dest = S3D::getSettingsFile(
			S3D::formatStringBuffer("/oldmods/%s-%u", newFileName.c_str(), time(0)));
		if (S3D::dirExists(src.c_str()))
		{
			if (0 == rename(src.c_str(), dest.c_str()))
			{
				S3D::dialogMessage("Scorched3D", S3D::formatStringBuffer(
					"Mod directory\n"
					"\"%s\"\n"
					"was moved to\n"
					"\"%s\"\n"
					"as it may be   compatibility incompatible with this version of Scorched3D",
					src.c_str(), dest.c_str()));

				return loadModFile(oldFileName.c_str(), global);
			}
		}
		else
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"Bundled mod \"%s\" has an out of date modinfo file.\n"
				"Mod version = %s, scorched version = %s",
				oldFileName.c_str(),
				newInfo.getProtocolVersion(), 
				S3D::ScorchedProtocolVersion.c_str()));
		}
	}

	dirs_.push_back(newInfo);
	return true;
}
