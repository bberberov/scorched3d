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

#if !defined(__INCLUDE_ScorchedServerSettingsh_INCLUDE__)
#define __INCLUDE_ScorchedServerSettingsh_INCLUDE__

#include <string>

class ScorchedServerSettings
{
public:
	ScorchedServerSettings(const std::string &type) :
		type_(type)
	{
	}
	virtual ~ScorchedServerSettings() 
	{
	}

	const std::string &type() { return type_; }

private:
	std::string type_;
};

class ScorchedServerSettingsOptions : public ScorchedServerSettings
{
public:
	ScorchedServerSettingsOptions(const std::string &settingsFile, 
		bool rewriteOptions, bool writeFullOptions) :
		ScorchedServerSettings("FILE"),
		settingsFile_(settingsFile),
		rewriteOptions_(rewriteOptions),
		writeFullOptions_(writeFullOptions)
	{
	}
	virtual ~ScorchedServerSettingsOptions()
	{
	}

	std::string settingsFile_;
	bool rewriteOptions_;
	bool writeFullOptions_;
};

class ScorchedServerSettingsSave : public ScorchedServerSettings
{
public:
	ScorchedServerSettingsSave(const std::string &saveFile) :
		ScorchedServerSettings("SAVE"),
		saveFile_(saveFile)
	{
	}
	virtual ~ScorchedServerSettingsSave()
	{
	}

	std::string saveFile_;
};

#endif // __INCLUDE_ScorchedServerSettingsh_INCLUDE__
