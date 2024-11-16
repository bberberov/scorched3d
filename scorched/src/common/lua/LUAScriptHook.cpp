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

#include <lua/LUAScriptHook.h>
#include <XML/XMLFile.h>
#include <common/FileList.h>
#include <common/Logger.h>
#ifndef S3D_SERVER
#include <console/ConsoleRuleMethodIAdapter.h>
#endif

LUAScriptHook::LUAScriptHook(LUAScriptFactory *factory, 
	const std::string &hooksName,
	const std::string &directoryName) :
	factory_(factory),
	hooksName_(hooksName),
	directoryName_(directoryName)
{
#ifndef S3D_SERVER

	new ConsoleRuleMethodIAdapter<LUAScriptHook>(
		this, &LUAScriptHook::clearHooks, "scriptHook", 
		ConsoleUtil::formParams(
			ConsoleRuleParam(hooksName_),
			ConsoleRuleParam("clear")));
	new ConsoleRuleMethodIAdapter<LUAScriptHook>(
		this, &LUAScriptHook::reloadHooks, "scriptHook", 
		ConsoleUtil::formParams(
			ConsoleRuleParam(hooksName_),
			ConsoleRuleParam("load")));
	new ConsoleRuleMethodIAdapter<LUAScriptHook>(
		this, &LUAScriptHook::listHooks, "scriptHook", 
		ConsoleUtil::formParams(
			ConsoleRuleParam(hooksName_),
			ConsoleRuleParam("list")));

#endif
}

LUAScriptHook::~LUAScriptHook()
{
}

void LUAScriptHook::addHookProvider(const std::string &hookName)
{
	hookNames_.insert(
		std::pair<std::string, std::vector<HookEntry> >
			(hookName, std::vector<HookEntry>()));
}

void LUAScriptHook::callHook(const std::string &hookName)
{
	std::vector<Param> result;
	callHookInternal(hookName, result);	
}

void LUAScriptHook::callHook(const std::string &hookName, const Param &param1)
{
	std::vector<Param> result;
	result.push_back(param1);
	callHookInternal(hookName, result);	
}

void LUAScriptHook::callHook(const std::string &hookName, const Param &param1, 
	const Param &param2)
{
	std::vector<Param> result;
	result.push_back(param1);
	result.push_back(param2);
	callHookInternal(hookName, result);	
}

void LUAScriptHook::callHook(const std::string &hookName, const Param &param1, 
	const Param &param2, const Param &param3)
{
	std::vector<Param> result;
	result.push_back(param1);
	result.push_back(param2);
	result.push_back(param3);
	callHookInternal(hookName, result);	
}

void LUAScriptHook::callHookInternal(const std::string &hookName, const std::vector<Param> &params)
{
	std::map<std::string, std::vector<HookEntry> >::iterator hookItor =
		hookNames_.find(hookName);
	if (hookItor == hookNames_.end()) return;

	std::vector<Param>::const_iterator paramItor;
	std::vector<Param>::const_iterator endParamItor = params.end();
	std::vector<HookEntry>::iterator 
		itor = hookItor->second.begin(), 
		endItor = hookItor->second.end();
	for (;itor != endItor;++itor)
	{
		LUAScript *script = itor->script;
		if (!script->startFunction(itor->entryPoint)) continue;

		for (paramItor = params.begin();
			paramItor != endParamItor;
			++paramItor)
		{
			switch (paramItor->type) 
			{
			case Param::eNumber:
				script->addNumberParameter(paramItor->number);
				break;
			case Param::eBoolean:
				script->addBoolParameter(paramItor->boolean);
				break;
			default:
				script->addStringParameter(paramItor->str);
				break;
			}
		}

		script->endFunction((int) params.size());
	}
}

void LUAScriptHook::listHooks()
{
	Logger::log("Hooks -----------------------------");
	std::map<std::string, std::vector<HookEntry> >::iterator topItor;
	for (topItor = hookNames_.begin();
		topItor != hookNames_.end();
		++topItor)
	{
		Logger::log(S3D::formatStringBuffer("  %s", topItor->first.c_str()));
	}
}

void LUAScriptHook::clearHooks()
{
	std::map<std::string, std::vector<HookEntry> >::iterator topItor;
	for (topItor = hookNames_.begin();
		topItor != hookNames_.end();
		++topItor)
	{
		std::vector<HookEntry>::iterator itor;
		for (itor = topItor->second.begin();
			itor != topItor->second.end();
			++itor)
		{
			HookEntry &entry = *itor;
			delete entry.script;
		}
		topItor->second.clear();
	}
}

bool LUAScriptHook::loadHooks()
{
	clearHooks();

	FileList fileList(directoryName_, "*.xml", true);

	FileList::ListType &files = fileList.getFiles();
	FileList::ListType::iterator itor;
	for (itor = files.begin();
		itor != files.end();
		++itor)
	{
		if (!loadHook(directoryName_, *itor)) return false;
	}

	return true;
}

bool LUAScriptHook::loadHook(const std::string &directoryName, const std::string &fileName)
{
	XMLFile file;
    if (!file.readFile(fileName))
	{
		S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
			"Failed to parse \"%s\"\n%s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode()) return true;
	
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 ++childrenItor)
    {
        XMLNode *currentNode = (*childrenItor);

		std::string scriptFileName, error;
		if (!currentNode->getNamedChild("filename", scriptFileName)) return false;

		if (scriptFileName.length() > 0 &&
			scriptFileName[0] != '/' &&
			scriptFileName[0] != '\\')
		{
			scriptFileName = directoryName + "/" + scriptFileName;
		}

		LUAScript *script = factory_->createScript();
		if (!script->loadFromFile(scriptFileName, error))
		{
			S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
				"From file %s, failed to parse LUA script \"%s\"\n%s", 
				fileName.c_str(),
				scriptFileName.c_str(),
				error.c_str()));
			return false;
		}

		XMLNode *hook = 0;
		while (currentNode->getNamedChild("hook", hook, false))
		{
			std::string hookName, entryPoint;
			if (!hook->getNamedChild("hookname", hookName)) return false;
			if (!hook->getNamedChild("entrypoint", entryPoint)) return false;

			std::map<std::string, std::vector<HookEntry> >::iterator hookItor =
				hookNames_.find(hookName);
			if (hookItor == hookNames_.end())
			{
				S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
					"From file %s, failed to find hook %s for LUA script \"%s\"", 
					fileName.c_str(),
					hookName.c_str(),
					scriptFileName.c_str()));
				return false;
			}

			if (!script->functionExists(entryPoint))
			{
				S3D::dialogMessage("LUAScriptHook", S3D::formatStringBuffer(
					"From file %s, failed to find entrypoint %s in LUA script \"%s\"", 
					fileName.c_str(),
					entryPoint.c_str(),
					scriptFileName.c_str()));
				return false;
			}

			HookEntry hookEntry = { script, entryPoint };
			hookItor->second.push_back(hookEntry);
		}
	}

	return true;
}
