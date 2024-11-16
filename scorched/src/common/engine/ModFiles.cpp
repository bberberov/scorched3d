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

#include <engine/ModFiles.h>
#include <engine/ModInfo.h>
#include <engine/ModFileEntryLoader.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/FileList.h>
#include <lang/LangResource.h>
#include <stdio.h>

ModFiles::ModFiles()
{
}

ModFiles::~ModFiles()
{
	clearAll();
}

bool ModFiles::fileEnding(const std::string &file, const std::string &ext)
{
	int fileLen = (int) file.size();
	int extLen = (int) ext.size();
	if (fileLen < extLen) return false;

	for (int i=0; i<extLen; i++)
	{
		if (file.c_str()[fileLen - i - 1] != ext.c_str()[extLen - i - 1])
		{
			return false;
		}
	}
	return true;
}

bool ModFiles::excludeSpecialFile(const std::string &file)
{
	if (file == "Thumbs.db") return true;
	if (file == "CVS") return true;
	if (file == "..") return true;
	if (strstr(file.c_str(), "..")) return true;

	return false;
}

bool ModFiles::excludeFile(const std::string &file)
{
	if (excludeSpecialFile(file)) return true;

	if (!fileEnding(file, ".xml") &&
		!fileEnding(file, ".bmp") &&
		!fileEnding(file, ".txt") &&
		!fileEnding(file, ".png") &&
		!fileEnding(file, ".jpg") &&
		!fileEnding(file, ".ico") &&
		!fileEnding(file, ".ase") &&
		!fileEnding(file, ".wav") &&
		!fileEnding(file, ".lua") &&
		!fileEnding(file, ".bw") &&
		!fileEnding(file, ".ogg"))
	{
		Logger::log(S3D::formatStringBuffer("Excluding mod file \"%s\"", file.c_str()));
		return true;
	}

	return false;
}

bool ModFiles::loadModFiles(const std::string &mod, 
	bool createDir, ProgressCounter *counter)
{
	NetBuffer tmpFileContents;

	clearAll();
	{
		// Get and check the user mod directory exists
		std::string modDir = S3D::getSettingsModFile(mod);
		if (S3D::dirExists(modDir))
		{
			if (counter) counter->setNewOp(LANG_RESOURCE_1("LOADING_USER_MODS", "Loading User Mod \"{0}\"", mod));
			if (!loadModDir(tmpFileContents, modDir, mod, counter)) return false;
		}
		else
		{
			if (createDir) S3D::dirMake(modDir);
		}
	}

	std::string modInfoFile = S3D::getModFile("data/modinfo.xml");
	ModInfo modInfo(mod.c_str());
	if (!modInfo.parse(modInfoFile))
	{
		S3D::dialogExit("Mod", S3D::formatStringBuffer(
			"Failed to parse mod info for mod %s", mod.c_str()));
	}

	{
		// Get and check global mod directory
		std::string modDir = S3D::getGlobalModFile(mod);
		if (S3D::dirExists(modDir))
		{
			if (counter) counter->setNewOp(LANG_RESOURCE_1("LOADING_GLOBAL_MODS", "Loading Global Mod \"{0}\"", mod));
			if (!loadModDir(tmpFileContents, modDir, mod, counter)) return false;
		}
	}
	
	{
		unsigned int totalSize = 0;
		std::map<std::string, ModFileEntry *>::iterator itor;
		for (itor = files_.begin();
			itor != files_.end();
			++itor)
		{
			ModFileEntry *entry = (*itor).second;
			totalSize += entry->getUncompressedSize();
		}

		Logger::log(S3D::formatStringBuffer("Loaded mod \"%s\", %u files, disk space required %u bytes", 
			mod.c_str(), files_.size(), totalSize));

		if (!createDir && files_.empty())
		{
			std::string modFile = S3D::getSettingsModFile(mod);
			std::string globalModFile = S3D::getGlobalModFile(mod);
			S3D::dialogMessage("Mod", S3D::formatStringBuffer(
				"Failed to find \"%s\" mod files in directories \"%s\" \"%s\"",
				mod.c_str(),
				modFile.c_str(),
				globalModFile.c_str()));
			return false;
		}
	}

	return true;
}

bool ModFiles::loadLocalModFile(NetBuffer &tmpFileContents, 
	const std::string &local, 
	const std::string &mod)
{
	std::string dataFile = S3D::getModFile(local);
	std::string modDirStr(dataFile.c_str());
	char *modDir = (char *) modDirStr.c_str();
	modDir[dataFile.size() - local.size()] = '\0';

	return loadModFile(tmpFileContents, dataFile, modDir, mod);
}

bool ModFiles::loadModDir(NetBuffer &tmpFileContents, 
	const std::string &modDir, 
	const std::string &mod, ProgressCounter *counter)
{
	// Load all files contained in this directory
	FileList allfiles(modDir, "*", true, true);
	FileList::ListType &files = allfiles.getFiles();
	FileList::ListType::iterator itor;

	int i = 0;
	for (itor = files.begin();
		itor != files.end();
		++itor, i++)
	{
		if (counter) counter->setNewPercentage(float(i) 
			/ float(files.size()) * 100.0f);

		// Load the file
		std::string &fileName = (*itor);
		if (!loadModFile(tmpFileContents, fileName, modDir, mod))
		{
			return false;
		}
	}

	return true;
}

bool ModFiles::loadModFile(NetBuffer &tmpFileContents, 
	const std::string &fullFileName,
	const std::string &modDir, const std::string &mod)
{
	std::string shortFileNameStr(fullFileName.c_str());
	S3D::fileDos2Unix(shortFileNameStr);
	const char *shortFileName = shortFileNameStr.c_str();

	// Check to see if we ignore this file
	if (excludeFile(fullFileName)) return true;

	// Turn it into a unix style path and remove the 
	// name of the directory that contains it
	int modDirLen = (int) modDir.size();
	shortFileName += modDirLen;
	while (shortFileName[0] == '/') shortFileName++;

	// Check that all files are lower case
	std::string oldFileName(shortFileName);
	_strlwr((char *) shortFileName);
	if (strcmp(oldFileName.c_str(), shortFileName) != 0)
	{
		S3D::dialogMessage("Mod", S3D::formatStringBuffer(
			"ERROR: All mod files must have lower case filenames.\n"
			"File \"%s,%s\" has upper case charaters in it",
			oldFileName.c_str(),
			shortFileName));
		return false;
	}

	// Check we don't have this file already
	if (files_.find(shortFileName) != files_.end()) return true;

	// Create the new mod file and load the file
	if (!ModFileEntryLoader::loadModFile(tmpFileContents, fullFileName))
	{
		S3D::dialogMessage("Mod", S3D::formatStringBuffer(
			"Error: Failed to load file \"%s\" mod directory \"%s\" in the \"%s\" mod",
			fullFileName.c_str(),
			modDir.c_str(),
			mod.c_str()));
		return false;
	}

	// Store for future
	ModFileEntry *file = new ModFileEntry(shortFileName, 
		tmpFileContents.getCrc(), 
		tmpFileContents.getBufferUsed());
	files_[shortFileName] = file;
	return true;
}

void ModFiles::clearAll()
{
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = files_.begin();
		itor != files_.end();
		++itor)
	{
		delete (*itor).second;
	}
	files_.clear();
}

bool ModFiles::exportModFiles(const std::string &mod, const std::string &fileName)
{
	FILE *out = fopen(fileName.c_str(), "wb");
	if (!out) return false;

	S3D::setDataFileMod(mod);

	// Mod Name
	NetBuffer tmpBuffer;
	tmpBuffer.reset();
	tmpBuffer.addToBuffer(S3D::ScorchedProtocolVersion);
	tmpBuffer.addToBuffer(mod);
	fwrite(tmpBuffer.getBuffer(),
		sizeof(unsigned char),
		tmpBuffer.getBufferUsed(), 
		out);	

	// Mod Files
	NetBuffer tmpFileContents;
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = files_.begin();
		itor != files_.end();
		++itor)
	{
		ModFileEntry *entry = (*itor).second;
	
		std::string fullFileName = S3D::getModFile(entry->getFileName());
		ModFileEntryLoader::loadModFile(tmpFileContents, fullFileName);

		tmpBuffer.reset();
		tmpBuffer.addToBuffer(entry->getFileName());
		tmpBuffer.addToBuffer(tmpFileContents.getBufferUsed());
		tmpBuffer.addToBuffer(tmpFileContents.getCrc());
		tmpBuffer.addDataToBuffer(tmpFileContents.getBuffer(),
			tmpFileContents.getBufferUsed());

		fwrite(tmpBuffer.getBuffer(),
			sizeof(unsigned char),
			tmpBuffer.getBufferUsed(), 
			out);
	}

	// End of Mod Files
	tmpBuffer.reset();
	tmpBuffer.addToBuffer("*");
	fwrite(tmpBuffer.getBuffer(),
		sizeof(unsigned char),
		tmpBuffer.getBufferUsed(), 
		out);

	fclose(out);
	return true;
}

bool ModFiles::importModFiles(std::string &mod, const std::string &fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");
	if (!in) return false;

	// Read Buffer
	NetBuffer tmpBuffer;
	unsigned char readBuf[512];
	while (unsigned int size = (unsigned int) fread(readBuf, sizeof(unsigned char), 512, in))
	{
		tmpBuffer.addDataToBuffer(readBuf, size);
	}
	fclose(in);
	return importModFiles(mod, tmpBuffer);
}

bool ModFiles::importModFiles(std::string &mod, NetBuffer &tmpBuffer)
{
	// Mod Name
	static std::string modName;
	std::string version;
	NetBufferReader tmpReader(tmpBuffer);
	if (!tmpReader.getFromBuffer(version)) return false;
	if (!tmpReader.getFromBuffer(modName)) return false;
	mod = modName;

	if (version != S3D::ScorchedProtocolVersion)
	{
		S3D::dialogMessage("Scorched3D", S3D::formatStringBuffer(
			"Failed to import mod, scorched version differs.\n"
			"Please obtain a newer version of this mod.\n"
			"Import version = %s\n"
			"Current version = %s\n",
			version.c_str(),
			S3D::ScorchedProtocolVersion.c_str()));
		return false;
	}

	std::string modDir = S3D::getSettingsModFile(mod);
	if (!S3D::dirExists(modDir))
	{
		S3D::dirMake(modDir);
	}

	NetBuffer tmpFileContents;
	for (;;)
	{
		// File Name
		std::string name;
		if (!tmpReader.getFromBuffer(name)) return false;
		if (0 == strcmp(name.c_str(), "*")) break;

		// File Header
		unsigned int uncompressedSize;
		unsigned int uncompressedCrc;
		tmpReader.getFromBuffer(uncompressedSize);
		tmpReader.getFromBuffer(uncompressedCrc);

		// File
		tmpFileContents.allocate(uncompressedSize);
		tmpFileContents.setBufferUsed(uncompressedSize);
		tmpReader.getDataFromBuffer(
			tmpFileContents.getBuffer(), 
			uncompressedSize);
		if (tmpFileContents.getCrc() != uncompressedCrc)
		{
			S3D::dialogMessage("Scorched3D", 
				S3D::formatStringBuffer("CRC failure for file %s in mod", name.c_str()));
			return false;
		}

		if (!ModFileEntryLoader::writeModFile(tmpFileContents, name, mod))
		{
			S3D::dialogMessage("Scorched3D", 
				S3D::formatStringBuffer("Failure to write mod file %s in mod", name.c_str()));
			return false;
		}
	}

	return true;
}
