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

#include <console/ConsoleRuleMethodIAdapter.h>
#include <console/ConsoleMethods.h>
#include <console/ConsoleFileReader.h>
#include <client/ScorchedClient.h>
#include <engine/MainLoop.h>

ConsoleMethods::ConsoleMethods()
{

}

ConsoleMethods::~ConsoleMethods()
{

}

void ConsoleMethods::init()
{
	new ConsoleRuleMethodIAdapter<ConsoleMethods>(
		this, &ConsoleMethods::clear, 
		"clear");
	new ConsoleRuleMethodIAdapter<ConsoleMethods>(
		this, &ConsoleMethods::exit, 
		"exit");
	new ConsoleRuleMethodIAdapter<ConsoleMethods>(
		this, &ConsoleMethods::exit, 
		"quit");
	new ConsoleRuleMethodIAdapter<ConsoleMethods>(
		this, &ConsoleMethods::help, 
		"help");
	new ConsoleRuleMethodIAdapterEx<ConsoleMethods>(
		this, &ConsoleMethods::consoleLoad, 
		"consoleload", 
		ConsoleUtil::formParams(ConsoleRuleParam("filename", ConsoleRuleTypeString)));
	new ConsoleRuleMethodIAdapterEx<ConsoleMethods>(
		this, &ConsoleMethods::consoleSave, 
		"consolesave", 
		ConsoleUtil::formParams(ConsoleRuleParam("filename", ConsoleRuleTypeString)));
}

void ConsoleMethods::clear()
{
	Console::instance()->clear();
}

void ConsoleMethods::help()
{
	Console::instance()->help();
}

void ConsoleMethods::exit()
{
	ScorchedClient::instance()->getMainLoop().exitLoop();
}

void ConsoleMethods::consoleLoad(std::vector<ConsoleRuleValue> &values)
{
	ConsoleRuleValue option = values[1];
	std::string errorString;
	if (!ConsoleFileReader::loadFileIntoConsole(
		option.valueString.c_str(), errorString))
	{
		Console::instance()->addLine(false, 
			S3D::formatStringBuffer("Failed to load to console \"%s\"",
			errorString.c_str()));
	}
}

void ConsoleMethods::consoleSave(std::vector<ConsoleRuleValue> &values)
{
	ConsoleRuleValue option = values[1];
	ConsoleFileReader::saveConsoleIntoFile(
		option.valueString.c_str());
}
