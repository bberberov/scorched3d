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

#if !defined(__INCLUDE_LangString_INCLUDE__)
#define __INCLUDE_LangString_INCLUDE__

#include <string>

#define LANG_STRING(x) LangStringUtil::convertToLang(x)

typedef unsigned int LangStringStorage;
typedef std::basic_string<LangStringStorage> LangString;

class LangStringConverter
{
public:
	LangStringConverter(const char *value); 
	LangStringConverter(const std::string &value); 
	LangStringConverter(const LangString &value); 
	LangStringConverter(const int value); 
	LangStringConverter(const unsigned int value); 
	LangStringConverter(const float value, int decimal = 0); 

	LangString &getValue() { return value_; }
	const LangString &getValue() const { return value_; }

	std::string getValueAsString();

private:
	LangString value_;

	void appendValue(const std::string &value);
};

namespace LangStringUtil
{
	LangString convertToLang(const LangStringConverter &input);
	void appendToLang(LangString &output, const LangStringConverter &input);
	void replaceToLang(LangString &output, const LangStringConverter &input);

	std::string convertFromLang(const LangString &input);

	void trim(LangString &str1);
	void lowercase(LangString &str1);
	int strcmp(const LangString &str1, const LangString &str2);
	int strcasecmp(const LangString &str1, const LangString &str2);
	int strlen(const LangString &str1);
	unsigned int *strstr(const unsigned int *str1, const LangString &str2);
	unsigned int *stristr(const unsigned int *str1, const LangString &str2);
};

#endif // __INCLUDE_LangString_INCLUDE__
