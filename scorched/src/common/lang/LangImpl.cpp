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

#include <lang/LangImpl.h>
#include <lang/ResourceBundleEntryImpl.h>
#include <common/Defines.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

static SDL_mutex *langMutex = 0;

LangImpl::LangImpl() : undefinedBundle_(0)
{
	langMutex = SDL_CreateMutex();
	init();
}

LangImpl::~LangImpl()
{
	clear();
}

void LangImpl::clear()
{
	std::vector<ResourceBundle *>::iterator itor;
	for (itor = bundles_.begin();
		itor != bundles_.end();
		++itor)
	{
		ResourceBundle *bundle = *itor;
		delete bundle;
	}
	bundles_.clear();
}

void LangImpl::init()
{
	SDL_LockMutex(langMutex);

	// Read the platform default language from the language file
	std::string currentLang = "EN";
	std::string langIniFile = S3D::getDataFile("data/lang/language.ini");
	FILE *in = fopen(langIniFile.c_str(), "r");
	if (in)
	{
		char buffer[256];
		if (fgets(buffer, 256, in)) 
		{
			for (int i=0; i<256; i++)
			{
				if (buffer[i] < ' ') buffer[i] = '\0';
			}
			currentLang = buffer;
		}
		fclose(in);
	}

	// Load the resource bundle for this language
	std::string langResourceFile =
		S3D::getDataFile(S3D::formatStringBuffer("data/lang/lang_%s.resource", 
			currentLang.c_str()));
	if (!S3D::fileExists(langResourceFile))
	{
		langResourceFile = S3D::getDataFile("data/lang/lang.resource");
	}
	ResourceBundle *langBundle = new ResourceBundle();
	langBundle->loadFromFile(langResourceFile);
	bundles_.push_back(langBundle);

	undefinedBundle_ = new ResourceBundle();
	bundles_.push_back(undefinedBundle_);
	SDL_UnlockMutex(langMutex);
}

void LangImpl::saveUndefined()
{
	undefinedBundle_->writeToFile(S3D::getDataFile("data/lang/lang_undefined.resource"));
}

ResourceBundleEntry *LangImpl::getEntry(
	const std::string &key, const std::string &value)
{
	return getEntry(key, LANG_STRING(value));
}

ResourceBundleEntry *LangImpl::getEntry(
	const std::string &key, const LangString &value)
{
	SDL_LockMutex(langMutex);

	ResourceBundleEntry *entry = 0;
	std::vector<ResourceBundle *>::iterator itor;
	for (itor = bundles_.begin();
		itor != bundles_.end();
		++itor)
	{
		ResourceBundle *bundle = *itor;
		entry = bundle->getEntry(key);
		if (entry) break;
	}
	if (!entry) 
	{
		entry = new ResourceBundleEntryImpl(key, value);
		undefinedBundle_->addEntry(entry);
	}

	SDL_UnlockMutex(langMutex);
	return entry;
}
