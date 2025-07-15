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

#if !defined(__INCLUDE_LangText_INCLUDE__)
#define __INCLUDE_LangText_INCLUDE__

#include <lang/Lang.h>

#define LANG_RESOURCE(key, value) LangResource::getKey(key, value)
#define LANG_RESOURCE_1(key, value, param1) LangResource::getKey(key, value, param1)
#define LANG_RESOURCE_2(key, value, param1, param2) LangResource::getKey(key, value, param1, param2)
#define LANG_RESOURCE_3(key, value, param1, param2, param3) LangResource::getKey(key, value, param1, param2, param3)
#define LANG_RESOURCE_4(key, value, param1, param2, param3, param4) LangResource::getKey(key, value, param1, param2, param3, param4)
#define LANG_RESOURCE_5(key, value, param1, param2, param3, param4, param5) LangResource::getKey(key, value, param1, param2, param3, param4, param5)

#define LANG_RESOURCE_VAR(name, key, value) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString();
#define LANG_RESOURCE_VAR_1(name, key, value, param1) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	LangString name = TEXT_BUNDLE_##name->getString(param1);
#define LANG_RESOURCE_VAR_2(name, key, value, param1, param2) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	LangString name = TEXT_BUNDLE_##name->getString(param1, param2);
#define LANG_RESOURCE_VAR_3(name, key, value, param1, param2, param3) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3);
#define LANG_RESOURCE_VAR_4(name, key, value, param1, param2, param3, param4) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3, param4);
#define LANG_RESOURCE_VAR_5(name, key, value, param1, param2, param3, param4, param5) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3, param4, param5);

#define LANG_RESOURCE_CONST_VAR(name, key, value) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString();
#define LANG_RESOURCE_CONST_VAR_1(name, key, value, param1) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString(param1);
#define LANG_RESOURCE_CONST_VAR_2(name, key, value, param1, param2) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString(param1, param2);
#define LANG_RESOURCE_CONST_VAR_3(name, key, value, param1, param2, param3) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3);
#define LANG_RESOURCE_CONST_VAR_4(name, key, value, param1, param2, param3, param4) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3, param4);
#define LANG_RESOURCE_CONST_VAR_5(name, key, value, param1, param2, param3, param4, param5) static ResourceBundleEntry *TEXT_BUNDLE_##name = \
	Lang::instance()->getEntry(key, value); \
	static LangString name = TEXT_BUNDLE_##name->getString(param1, param2, param3, param4, param5);

namespace LangResource
{
	LangString getKey(const std::string &key, const std::string &value);
	LangString getKey(const std::string &key, const std::string &value, const LangStringConverter &param1);
	LangString getKey(const std::string &key, const std::string &value, const LangStringConverter &param1, 
		const LangStringConverter &param2);
	LangString getKey(const std::string &key, const std::string &value, const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3);
	LangString getKey(const std::string &key, const std::string &value, const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3, const LangStringConverter &param4);
	LangString getKey(const std::string &key, const std::string &value, const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3, const LangStringConverter &param4,
		const LangStringConverter &param5);
}

#endif // __INCLUDE_LangText_INCLUDE__
