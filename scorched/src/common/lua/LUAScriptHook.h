////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#if !defined(__INCLUDE_LUAScriptHook_INCLUDE__)
#define __INCLUDE_LUAScriptHook_INCLUDE__

#include "LUAScriptFactory.h"
#include <lang/LangString.h>
#include <map>
#include <vector>

class LUAScriptHook
{
public:
	class Param
	{
	public:
		enum Type
		{
			eString,
			eNumber,
			eBoolean
		};

		Param(fixed innumber) :
			type(eNumber),
			number(innumber)
		{};
		Param(const char *instr) :
			type(eString),
			str(instr)
		{};
		Param(const std::string &instr) :
			type(eString),
			str(instr)
		{};
		Param(const LangString &instr) :
			type(eString),
			str( LangStringUtil::convertFromLang(instr) )
		{};
		Param(bool b) :
			type(eBoolean),
			boolean(b)
		{};

		Type type;
		bool boolean;
		fixed number;
		std::string str;
	};

	LUAScriptHook(LUAScriptFactory *factory, const std::string &hooksName, const std::string &directoryName);
	~LUAScriptHook();

	void addHookProvider(const std::string &hookName);

	void callHook(const std::string &hookName);
	void callHook(const std::string &hookName, const Param &param1);
	void callHook(const std::string &hookName, const Param &param1, const Param &param2);
	void callHook(const std::string &hookName, const Param &param1, const Param &param2, const Param &param3);
	
	void clearHooks();
	bool loadHooks();
	void listHooks();

protected:
	struct HookEntry
	{
		LUAScript *script;
		std::string entryPoint;
	};

	LUAScriptFactory *factory_;
	std::string hooksName_;
	std::string directoryName_;
	std::map<std::string, std::vector<HookEntry> > hookNames_;
	bool loadHook(const std::string &directoryName, const std::string &fileName);
	void reloadHooks() { loadHooks(); }

	void callHookInternal(const std::string &hookName, const std::vector<Param> &params);
};

#endif // __INCLUDE_LUAScriptHook_INCLUDE__
