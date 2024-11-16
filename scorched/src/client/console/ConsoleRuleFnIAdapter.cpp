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
#include <console/Console.h>
#include <console/ConsoleRuleFnIAdapter.h>

ConsoleRuleFnIBooleanAdapter::ConsoleRuleFnIBooleanAdapter(const char *name, bool &param) : 
	name_(name), param_(param),
	readRule_(0), writeRule_(0)
{
	readRule_ = new ConsoleRuleFn(name, this, ConsoleRuleTypeBoolean, false);
	writeRule_ = new ConsoleRuleFn(name, this, ConsoleRuleTypeBoolean, true);
	Console::instance()->addRule(readRule_);
	Console::instance()->addRule(writeRule_);
}

ConsoleRuleFnIBooleanAdapter::~ConsoleRuleFnIBooleanAdapter()
{
	Console::instance()->removeRule(readRule_);
	Console::instance()->removeRule(writeRule_);
	delete readRule_;
	delete writeRule_;
}

bool ConsoleRuleFnIBooleanAdapter::getBoolParam(const char *name)
{
	DIALOG_ASSERT(name_ == name);
	return param_;
}

void ConsoleRuleFnIBooleanAdapter::setBoolParam(const char *name, bool value)
{
	DIALOG_ASSERT(name_ == name);
	param_ = value;
}

ConsoleRuleFnINumberAdapter::ConsoleRuleFnINumberAdapter(const char *name, float &param) : 
	name_(name), param_(param),
	readRule_(0), writeRule_(0)
{
	readRule_ = new ConsoleRuleFn(name, this, ConsoleRuleTypeNumber, false);
	writeRule_ = new ConsoleRuleFn(name, this, ConsoleRuleTypeNumber, true);
	Console::instance()->addRule(readRule_);
	Console::instance()->addRule(writeRule_);
}

ConsoleRuleFnINumberAdapter::~ConsoleRuleFnINumberAdapter()
{
	Console::instance()->removeRule(readRule_);
	Console::instance()->removeRule(writeRule_);
	delete readRule_;
	delete writeRule_;
}

float ConsoleRuleFnINumberAdapter::getNumberParam(const char *name)
{
	DIALOG_ASSERT(name_ == name);
	return param_;
}

void ConsoleRuleFnINumberAdapter::setNumberParam(const char *name, float value)
{
	DIALOG_ASSERT(name_ == name);
	param_ = value;
}

ConsoleRuleFnIOptionsAdapter::ConsoleRuleFnIOptionsAdapter(OptionEntry &entry, bool write) :
	entry_(entry),
	readRule_(0), writeRule_(0)
{
	ConsoleRuleType type = ConsoleRuleTypeNone;
	switch (entry.getEntryType())
	{
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryIntType:
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryFloatType:
		case OptionEntry::OptionEntryFixedType:
			type = ConsoleRuleTypeNumber;
		break;
		case OptionEntry::OptionEntryVectorType:
		case OptionEntry::OptionEntryStringEnumType:
		case OptionEntry::OptionEntryStringType:
		case OptionEntry::OptionEntryTextType:
			type = ConsoleRuleTypeString;
		break;
		case OptionEntry::OptionEntryBoolType:
			type = ConsoleRuleTypeBoolean;
		break;
		default:
			DIALOG_ASSERT(0);
		break;
	}

	readRule_ = new ConsoleRuleFn(entry.getName(), this, type, false);
	Console::instance()->addRule(readRule_);
	if (write) 
	{
		writeRule_ = new ConsoleRuleFn(entry.getName(), this, type, true);
		Console::instance()->addRule(writeRule_);
	}
}

ConsoleRuleFnIOptionsAdapter::~ConsoleRuleFnIOptionsAdapter()
{
	if (readRule_)
	{
		Console::instance()->removeRule(readRule_);
		delete readRule_;
	}
	if (writeRule_)
	{
		Console::instance()->removeRule(writeRule_);
		delete writeRule_;
	}
}

bool ConsoleRuleFnIOptionsAdapter::getBoolParam(const char *name)
{
	return ((OptionEntryBool &) entry_).getValue();
}

void ConsoleRuleFnIOptionsAdapter::setBoolParam(const char *name, bool value)
{
	((OptionEntryBool &) entry_).setValue(value);
}

float ConsoleRuleFnIOptionsAdapter::getNumberParam(const char *name)
{
	if (entry_.getEntryType() == OptionEntry::OptionEntryIntType)
	{
		return (float) (((OptionEntryInt &) entry_).getValue());
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFloatType)
	{
		return ((OptionEntryFloat &) entry_).getValue();
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFixedType)
	{
		return ((OptionEntryFixed&) entry_).getValue().asFloat();
	}
	return -99.99f;
}

void  ConsoleRuleFnIOptionsAdapter::setNumberParam(const char *name, float value)
{
	if (entry_.getEntryType() == OptionEntry::OptionEntryIntType)
	{
		((OptionEntryInt &) entry_).setValue(int(value));
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFloatType)
	{
		((OptionEntryFloat &) entry_).setValue(value);
	}
	else if (entry_.getEntryType() == OptionEntry::OptionEntryFixedType)
	{
		((OptionEntryFixed &) entry_).setValue(fixed(int(value)));
	}
}

const char *ConsoleRuleFnIOptionsAdapter::getStringParam(const char *name)
{
	return ((OptionEntryString &) entry_).getValue();
}

void ConsoleRuleFnIOptionsAdapter::setStringParam(const char *name, const char *value)
{
	((OptionEntryString &) entry_).setValue(value);
}
