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

#if !defined(__INCLUDE_ResourceBundle_INCLUDE__)
#define __INCLUDE_ResourceBundle_INCLUDE__

#include <set>
#include <string.h>
#include <lang/ResourceBundleEntry.h>

class ResourceBundle
{
public:
	ResourceBundle() {}
	~ResourceBundle();

	bool loadFromFile(const std::string &file);
	bool writeToFile(const std::string &file);

	ResourceBundleEntry *getEntry(const std::string &key);
	void addEntry(ResourceBundleEntry *entry);

private:
	struct LessThanResourceBundleEntry
	{
		bool operator()(ResourceBundleEntry* s1, 
			ResourceBundleEntry* s2) const
		{
			return strcmp(s1->getKey(), s2->getKey()) < 0;
		}
	};
	
	typedef std::set<ResourceBundleEntry*, LessThanResourceBundleEntry> ResourceBundleSet;
	ResourceBundleSet entries_;
};

#endif // __INCLUDE_ResourceBundle_INCLUDE__
