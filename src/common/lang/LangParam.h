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

#if !defined(__INCLUDE_LangParam_INCLUDE__)
#define __INCLUDE_LangParam_INCLUDE__

#include <lang/LangString.h>
#include <vector>

#define LANG_PARAM_1(key, param1) \
	LangParam::getKey(key, param1)
#define LANG_PARAM_2(key, param1, param2) \
	LangParam::getKey(key, param1, param2)
#define LANG_PARAM_3(key, param1, param2, param3) \
	LangParam::getKey(key, param1, param2, param3)
#define LANG_PARAM_4(key, param1, param2, param3, param4) \
	LangParam::getKey(key, param1, param2, param3, param4)
#define LANG_PARAM_5(key, param1, param2, param3, param4, param5) \
	LangParam::getKey(key, param1, param2, param3, param4, param5)

class LangParam
{
public:
	LangParam(const LangStringConverter &value);

	LangString getValue();

	LangString getParameterizedString();
	LangString getParameterizedString(const LangStringConverter &param1);
	LangString getParameterizedString(const LangStringConverter &param1, 
		const LangStringConverter &param2);
	LangString getParameterizedString(const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3);
	LangString getParameterizedString(const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3, 
		const LangStringConverter &param4);
	LangString getParameterizedString(const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3, 
		const LangStringConverter &param4, const LangStringConverter &param5);

	static LangString getKey(const LangStringConverter &value,
		const LangStringConverter &param1);
	static LangString getKey(const LangStringConverter &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2);
	static LangString getKey(const LangStringConverter &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2,
		const LangStringConverter &param3);
	static LangString getKey(const LangStringConverter &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2,
		const LangStringConverter &param3,
		const LangStringConverter &param4);
	static LangString getKey(const LangStringConverter &value,
		const LangStringConverter &param1,
		const LangStringConverter &param2,
		const LangStringConverter &param3,
		const LangStringConverter &param4,
		const LangStringConverter &param5);

private:
	std::vector<LangString> parts_;
	std::vector<int> positions_;
};

#endif // __INCLUDE_LangParam_INCLUDE__
