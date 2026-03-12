////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_ConsoleRuleFnIAdapter_hpp_INCLUDE__
#define __INCLUDE_ConsoleRuleFnIAdapter_hpp_INCLUDE__

#include <common/OptionEntry.hpp>
#include <console/ConsoleRuleFn.hpp>
#include <string>

class ConsoleRuleFnIBooleanAdapter : public ConsoleRuleFnI
{
public:
	ConsoleRuleFnIBooleanAdapter( const char* name, bool& param );
	virtual ~ConsoleRuleFnIBooleanAdapter();

	// Inherited from ConsoleRuleFnI
	virtual bool getBoolParam( const char* name );
	virtual void setBoolParam( const char* name, bool value );

protected:
	std::string    name_;
	bool&          param_;
	ConsoleRuleFn* readRule_;
	ConsoleRuleFn* writeRule_;
};

class ConsoleRuleFnINumberAdapter : public ConsoleRuleFnI
{
public:
	ConsoleRuleFnINumberAdapter( const char* name, float& param );
	virtual ~ConsoleRuleFnINumberAdapter();

	// Inherited from ConsoleRuleFnI
	virtual float getNumberParam( const char* name );
	virtual void  setNumberParam( const char* name, float value );

protected:
	std::string    name_;
	float&         param_;
	ConsoleRuleFn* readRule_;
	ConsoleRuleFn* writeRule_;
};

class ConsoleRuleFnIOptionsAdapter : public ConsoleRuleFnI
{
public:
	ConsoleRuleFnIOptionsAdapter( OptionEntry& entry, bool write = false );
	virtual ~ConsoleRuleFnIOptionsAdapter();

	// Inherited from ConsoleRuleFnI
	virtual bool        getBoolParam( const char* name );
	virtual void        setBoolParam( const char* name, bool value );
	virtual float       getNumberParam( const char* name );
	virtual void        setNumberParam( const char* name, float value );
	virtual const char* getStringParam( const char* name );
	virtual void        setStringParam( const char* name, const char* value );

protected:
	OptionEntry&   entry_;
	ConsoleRuleFn* readRule_;
	ConsoleRuleFn* writeRule_;
};

#endif  // __INCLUDE_ConsoleRuleFnIAdapter_hpp_INCLUDE__
