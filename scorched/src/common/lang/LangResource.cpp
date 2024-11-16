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

#include <lang/LangResource.h>

LangString LangResource::getKey(const std::string &key, const std::string &value)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString();
	return result;
}

LangString LangResource::getKey(const std::string &key, const std::string &value, 
	const LangStringConverter &param1)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString(param1);
	return result;
}

LangString LangResource::getKey(const std::string &key, const std::string &value, 
	const LangStringConverter &param1, const LangStringConverter &param2)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString(param1, param2);
	return result;
}

LangString LangResource::getKey(const std::string &key, const std::string &value, 
	const LangStringConverter &param1, const LangStringConverter &param2, const LangStringConverter &param3)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString(param1, param2, param3);
	return result;
}

LangString LangResource::getKey(const std::string &key, const std::string &value, 
	const LangStringConverter &param1, const LangStringConverter &param2, const LangStringConverter &param3, 
	const LangStringConverter &param4)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString(param1, param2, param3, param4);
	return result;
}

LangString LangResource::getKey(const std::string &key, const std::string &value, 
	const LangStringConverter &param1, const LangStringConverter &param2, const LangStringConverter &param3, 
	const LangStringConverter &param4, const LangStringConverter &param5)
{
	ResourceBundleEntry *bundle = Lang::instance()->getEntry(key, value);
	LangString result = bundle->getString(param1, param2, param3, param4, param5);
	return result;
}

static void testMacro()
{
	LANG_RESOURCE("hmm", "hmm value");

	LANG_RESOURCE_VAR(bob_0, "test", "test value");
	bob_0.c_str();

	LANG_RESOURCE_VAR_1(bob_1, "test", "test value {0}", "1");
	bob_1.c_str();

	LANG_RESOURCE_VAR_2(bob_2, "test", "test value {0} {1}", "1", "2");
	bob_2.c_str();

	LANG_RESOURCE_VAR_3(bob_3, "test", "test value {0} {1} {2}", "1", "2", "3");
	bob_3.c_str();

	LANG_RESOURCE_VAR_4(bob_4, "test", "test value {0} {1} {2} {3}", "1", "2", "3", "4");
	bob_4.c_str();

	LANG_RESOURCE_CONST_VAR(bob_c_0, "test", "test value");
	bob_c_0.c_str();

	LANG_RESOURCE_CONST_VAR_1(bob_c_1, "test {0}", "test value", "1");
	bob_c_1.c_str();

	LANG_RESOURCE_CONST_VAR_2(bob_c_2, "test {0} {1}", "test value", "1", "2");
	bob_c_2.c_str();

	LANG_RESOURCE_CONST_VAR_3(bob_c_3, "test {0} {1} {2}", "test value", "1", "2", "3");
	bob_c_3.c_str();

	LANG_RESOURCE_CONST_VAR_4(bob_c_4, "test {0} {1} {2} {3}", "test value", "1", "2", "3", "4");
	bob_c_4.c_str();
}
