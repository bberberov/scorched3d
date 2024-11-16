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

#include <common/Defines.h>
#include <engine/ModFileEntryLoader.h>
#include <engine/ModFiles.h>

bool ModFileEntryLoader::writeModFile(NetBuffer &buffer, 
	const std::string &fileName, const std::string &modName)
{
	// Check that this file is allowed to be sent
	if (ModFiles::excludeFile(fileName)) return true;

	// Create any needed directories
	char *dir = (char *) fileName.c_str();
	while (dir = strchr(dir, '/'))
	{
		*dir = '\0';
		std::string needdir = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
			modName.c_str(), fileName.c_str()));
		if (!S3D::dirExists(needdir)) S3D::dirMake(needdir);
		*dir = '/';
		dir++;
	}

	// Write the file 
	std::string needfile = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
		modName.c_str(), fileName.c_str()));
	FILE *file = fopen(needfile.c_str(), "wb");
	if (!file)
	{
		S3D::dialogMessage("WriteModFile",
			"Create file error");
		return false;
	}
	if (fwrite(buffer.getBuffer(), sizeof(unsigned char), 
		buffer.getBufferUsed(), file) != buffer.getBufferUsed())
	{
		S3D::dialogMessage("WriteModFile",
			"Write file error");
		fclose(file);
		return false;
	}
	fclose(file);

	return true;
}

bool ModFileEntryLoader::removeModFile(const std::string &filename, const std::string &modName)
{
	// Check that this file is allowed to be sent
	if (ModFiles::excludeFile(filename)) return true;

	std::string needfile = S3D::getSettingsModFile(S3D::formatStringBuffer("%s/%s", 
		modName.c_str(), filename.c_str()));
	S3D::removeFile(needfile);

	return true;
}

bool ModFileEntryLoader::loadModFile(NetBuffer &buffer, const std::string &filename)
{
	buffer.reset();
	{
		// Do translation on ASCII files to prevent CVS
		// from being a biatch
		bool translate = false;
		if (ModFiles::fileEnding(filename, ".txt") ||
			ModFiles::fileEnding(filename, ".xml"))
		{
			translate = true;
		}

		// Load the file into a coms buffer
		FILE *file = fopen(filename.c_str(), "rb");
		if (!file) return false;
		int newSize = 0;
		unsigned char strbuffer[256];
		static NetBuffer tmpBuffer;
		tmpBuffer.reset();
		do
		{
			newSize = (int) fread(strbuffer, 
				sizeof(unsigned char), 
				sizeof(buffer), 
				file);
			tmpBuffer.addDataToBuffer(strbuffer, newSize);
		}
		while (newSize > 0);
		fclose(file);

		if (!translate)
		{
			buffer.addDataToBuffer(
				tmpBuffer.getBuffer(),
				tmpBuffer.getBufferUsed());
		}
		else
		{
			for (unsigned i=0; i<tmpBuffer.getBufferUsed(); i++)
			{
				if (i >= tmpBuffer.getBufferUsed() - 1 ||
					tmpBuffer.getBuffer()[i] != 13 ||
					tmpBuffer.getBuffer()[i+1] != 10)
				{	
					buffer.addDataToBuffer(
						&tmpBuffer.getBuffer()[i] , 1);
				}
			}
		}
	}

	return true;
}
