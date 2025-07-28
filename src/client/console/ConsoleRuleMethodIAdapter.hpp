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

#if !defined(__INCLUDE_ConsoleRuleMethodIAdapterh_INCLUDE__)
#define __INCLUDE_ConsoleRuleMethodIAdapterh_INCLUDE__

#include <console/Console.h>

// Q. Hmm what happens when you have a long winded naming scheme?
// A. name -> inf

// Usage pattern :-
// 	new ConsoleRuleMethodIAdapter<InfoMap>(
//		this, showHeightBands, "InfoHeightBands");
// Calls InfoMap::showHeightBands when InfoHeightBands is selected 
// in the console
template<class T>
class ConsoleRuleMethodIAdapter : public ConsoleRule
{
public:
	ConsoleRuleMethodIAdapter(T *inst, 
		void (T::*call)(), 
		const char *name) : 
		ConsoleRule(name, std::vector<ConsoleRuleParam>()), inst_(inst), call_(call)
	{
		Console::instance()->addRule(this);
	};

	ConsoleRuleMethodIAdapter(T *inst, 
		void (T::*call)(), 
		const char *name, const std::vector<ConsoleRuleParam> &params) : 
		ConsoleRule(name, params), inst_(inst), call_(call)
	{
		Console::instance()->addRule(this);
	};
	virtual ~ConsoleRuleMethodIAdapter()
	{
		Console::instance()->removeRule(this);
	};

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values)
	{
		(inst_->*call_)();
	};

protected:
	std::string name_;
	T *inst_;
	void (T::*call_)();
};

// Same as above but passed params to method
template<class T>
class ConsoleRuleMethodIAdapterEx : public ConsoleRule
{
public:
	ConsoleRuleMethodIAdapterEx(T *inst, 
		void (T::*call)(std::vector<ConsoleRuleValue>&), 
		const char *name, const std::vector<ConsoleRuleParam> &params) :
		ConsoleRule(name, params), inst_(inst), call_(call)
	{
		Console::instance()->addRule(this);
	};
	virtual ~ConsoleRuleMethodIAdapterEx()
	{
		Console::instance()->removeRule(this);
	};

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values)
	{
		(inst_->*call_)(values);
	};

protected:
	std::string name_;
	T *inst_;
	void (T::*call_)(std::vector<ConsoleRuleValue>&);
};

// Same as above but passed userdata to method
template<class T>
class ConsoleRuleMethodIAdapterEx2 : public ConsoleRule
{
public:
	ConsoleRuleMethodIAdapterEx2(T *inst, 
		void (T::*call)(std::vector<ConsoleRuleValue>&, unsigned int), 
		const char *name, const std::vector<ConsoleRuleParam> &params,
		unsigned int userData) :
		ConsoleRule(name, params, userData), inst_(inst), call_(call)
	{
		Console::instance()->addRule(this);
	};
	virtual ~ConsoleRuleMethodIAdapterEx2()
	{
		Console::instance()->removeRule(this);
	};

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values)
	{
		(inst_->*call_)(values, userData_);
	};

protected:
	std::string name_;
	T *inst_;
	void (T::*call_)(std::vector<ConsoleRuleValue>&, unsigned int);
};

#endif

