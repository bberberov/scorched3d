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

#if !defined(AFX_ConsoleRULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_)
#define AFX_ConsoleRULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_

#include <console/ConsoleRule.h>
#include <common/DefinesAssert.h>

class ConsoleRuleFnI
{
public:
	virtual ~ConsoleRuleFnI();

	virtual bool getBoolParam(const char *name) { DIALOG_ASSERT(0); return true; }
	virtual void setBoolParam(const char *name, bool value) { DIALOG_ASSERT(0); }

	virtual float getNumberParam(const char *name) { DIALOG_ASSERT(0); return 0.0f; }
	virtual void  setNumberParam(const char *name, float value) { DIALOG_ASSERT(0); }

	virtual const char *getStringParam(const char *name) { DIALOG_ASSERT(0); return 0; }
	virtual void setStringParam(const char *name, const char *value) { DIALOG_ASSERT(0); }
};

class ConsoleRuleFn : public ConsoleRule
{
public:
	ConsoleRuleFn(const char *name, 
		ConsoleRuleFnI *user, 
		ConsoleRuleType type, 
		bool write = false);
	virtual ~ConsoleRuleFn();

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values);

protected:
	ConsoleRuleFnI *user_;
	ConsoleRuleType type_;

	void setValue(ConsoleRuleValue &split);
	const char *getValue();
};

#endif // !defined(AFX_ConsoleRULEFN_H__B0DEC68E_5D12_410F_AEA2_3BF1AF6B3319__INCLUDED_)
