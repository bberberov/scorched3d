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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <cstdlib>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static ARGParser aParser;
char scorched3dAppName[128];

// Compilers from Borland report floating-point exceptions in a manner 
// that is incompatible with Microsoft Direct3D.
int _matherr(struct _exception  *e)
{
    e;               // Dummy reference to catch the warning.
    return 1;        // Error has been handled.
}

void _no_storage()
{
	printf("Failed to allocate memory!!");
	std::exit(1);
}

void run_main(int argc, char *argv[], OptionsParameters &params)
{
	std::set_new_handler(&_no_storage);

	// Set the path the executable was run with
	S3D::setExeName((const char *) argv[0]);

	// Generate the version
	snprintf(scorched3dAppName, 128, "Scorched3D - Version %s (%s)", 
		S3D::ScorchedVersion.c_str(), S3D::ScorchedProtocolVersion.c_str());

	srand((unsigned)time(0));

	// Parse command line
	// Read options from command line
	if (!OptionEntryHelper::addToArgParser(
		params.getOptions(), aParser)) exit(64);
	std::list<OptionEntry *>::iterator nonParamItor;
	for (nonParamItor = params.getNonParamOptions().begin();
		nonParamItor != params.getNonParamOptions().end();
		++nonParamItor)
	{
		OptionEntryString *str = (OptionEntryString *) *nonParamItor;
		aParser.addNonParamEntry(
			(char *) str->getName(), str, 
			(char *) str->getDescription());
	}
	if (!aParser.parse(argc, argv)) exit(64);
	S3D::setSettingsDir(params.getSettingsDir());

	// Check we are in the correct directory
	std::string fileName = S3D::getDataFile("data/autoexec.xml");
	FILE *checkfile = fopen(fileName.c_str(), "r");
	if (!checkfile)
	{
		// Perhaps we can get the directory from the executables path name
		std::string path = argv[0];
		S3D::fileDos2Unix(path);
		size_t slashPos = path.rfind('/');
		if (slashPos != std::string::npos)
		{
			path.erase(slashPos);
#ifdef _WIN32
			SetCurrentDirectory(path.c_str());
#else
			chdir(path.c_str());
#endif // _WIN32			
		}

		// Now try again for the correct directory
		std::string execFile = S3D::getDataFile("data/autoexec.xml");
		checkfile = fopen(execFile.c_str(), "r");
		if (!checkfile)
		{	
			static char currentDir[1024];
#ifdef _WIN32
			GetCurrentDirectory(sizeof(currentDir), currentDir);
#else
			getcwd(currentDir, sizeof(currentDir));
#endif // _WIN32
			std::string dataPath = S3D::getDataFile("data");
			S3D::dialogExit(
				scorched3dAppName, S3D::formatStringBuffer(
				"Error: This game requires the Scorched3D data directory to run.\n"
				"Your machine does not appear to have the Scorched3D data directory in\n"
				"the required location.\n"
				"The data directory is set to \"%s\" which does not exist.\n"
				"(Current working directory %s)\n\n"
				"If Scorched3D does not run please re-install Scorched3D.",
				dataPath.c_str(), currentDir));
		}
	}
	else fclose(checkfile);

	// Check that the mods are uptodate with the current scorched3d
	// version
	ModDirs dirs;
	dirs.loadModDirs();

#ifndef _WIN32
	// Tells Linux not to issue a sig pipe when writting to a closed socket
	// Why does it have to be dificult!
	signal(SIGPIPE, SIG_IGN);
	signal(SIGFPE, SIG_IGN);
#endif

	if (setlocale(LC_ALL, "C") == 0)
	{
		S3D::dialogMessage(
			scorched3dAppName,
			"Warning: Failed to set client locale");
	}
}
