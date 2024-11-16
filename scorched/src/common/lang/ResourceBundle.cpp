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

#include <lang/ResourceBundle.h>
#include <lang/ResourceBundleEntryImpl.h>
#include <common/DefinesString.h>

ResourceBundle::~ResourceBundle()
{
	ResourceBundleSet::iterator itor;
	for (itor = entries_.begin(); itor != entries_.end(); ++itor)
	{
		delete *itor;
	}
	entries_.clear();
}

ResourceBundleEntry *ResourceBundle::getEntry(const std::string &key)
{
	ResourceBundleEntryImpl searchEntry(key);
	ResourceBundleSet::iterator findItor = entries_.find(&searchEntry);
	if (findItor == entries_.end()) return 0;
	return *findItor;
}

void ResourceBundle::addEntry(ResourceBundleEntry *entry)
{
	ResourceBundleSet::iterator findItor = entries_.find(entry);
	if (findItor != entries_.end())
	{
		entries_.erase(findItor);
		delete *findItor;
	}
	entries_.insert(entry);
}

bool ResourceBundle::loadFromFile(const std::string &file)
{
	FILE *in = fopen(file.c_str(), "r");
	if (!in) return false;

	char buffer[2048];
	while (fgets(buffer, 2048, in) != 0)
	{
		std::string key, value;

		bool inKey = true;
		for (char *start=buffer; *start; start++)
		{			
			if (inKey) 
			{
				if (*start == '=') inKey = false;
				else key.push_back(*start);
			}
			else value.push_back(*start);
		}
		S3D::trim(key);
		S3D::trim(value);
		if (key.size() == 0) continue;
		if (key[0] == '#' || key[0] == '\\' || key[0] == ';') continue;

		LangString langValue;
		LangStringUtil::appendToLang(langValue, value);

		ResourceBundleEntry *entry = new ResourceBundleEntryImpl(key, langValue);
		addEntry(entry);
	}
	fclose(in);

	return true;
}

bool ResourceBundle::writeToFile(const std::string &file)
{
	FILE *out = fopen(file.c_str(), "w");
	if (!out) return false;

	ResourceBundleSet::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		ResourceBundleEntry *entry = *itor;

		std::string value = LangStringUtil::convertFromLang(entry->getValue());
		fprintf(out, "%s = %s\n", entry->getKey(), value.c_str());
	}

	fclose(out);
	return true;
}
