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

#include <lang/LangString.h>
#include <common/DefinesString.h>

LangStringConverter::LangStringConverter(const char *value)
{
	appendValue(value);
}

LangStringConverter::LangStringConverter(const std::string &value)
{
	appendValue(value);
}

LangStringConverter::LangStringConverter(const LangString &value) :
	value_(value)
{
}

LangStringConverter::LangStringConverter(const int value)
{
	appendValue(S3D::formatStringBuffer("%i", value));
}

LangStringConverter::LangStringConverter(const unsigned int value)
{
	appendValue(S3D::formatStringBuffer("%u", value));
}

LangStringConverter::LangStringConverter(const float value, int decimal)
{
	switch (decimal)
	{
	case 1:
		appendValue(S3D::formatStringBuffer("%.1f", value));
		break;
	case 2:
		appendValue(S3D::formatStringBuffer("%.2f", value));
		break;
	default:
		appendValue(S3D::formatStringBuffer("%.0f", value));
		break;
	}
}

void LangStringConverter::appendValue(const std::string &input)
{
	for (const char *i=input.c_str(); *i; i++)
	{
		if (*i == '\\')
		{
			const unsigned int next = *(i+1);
			switch (next)
			{
			case '\\':
				value_.push_back('\\');
				i++;
				break;
			case 'n':
				value_.push_back('\n');
				i++;
				break;
			case 't':
				value_.push_back('\t');
				i++;
				break;
			case 'u':
				{
					char a[] = { *(i+2), *(i+3), *(i+4), *(i+5), '\0' };
					unsigned int value;
					sscanf(a, "%04X", &value);
					value_.push_back(value);
					i+=5;
				}
				break;
			}
		}
		else
		{
			value_.push_back(*i);
		}
	}
}

std::string LangStringConverter::getValueAsString()
{
	std::string result;
	for (const unsigned int *i=value_.c_str(); *i; i++)
	{
		if (*i < 32 || *i > 126)
		{
			if (*i == '\n') result.append("\\n");
			else if (*i == '\\') result.append("\\\\");
			else if (*i == '\t') result.append("\\t");
			else result.append(S3D::formatStringBuffer("\\u%04X", *i));
		}
		else
		{
			result.push_back((char) *i);
		}
	}
	
	return result;
}

LangString LangStringUtil::convertToLang(const LangStringConverter &input)
{
	return input.getValue();
}

void LangStringUtil::appendToLang(LangString &output, const LangStringConverter &input)
{
	output.append(input.getValue());
}

void LangStringUtil::replaceToLang(LangString &output, const LangStringConverter &input)
{
	output.clear();
	output.append(input.getValue());
}

std::string LangStringUtil::convertFromLang(const LangString &input)
{
	LangStringConverter converter(input);
	return converter.getValueAsString();
}

void LangStringUtil::lowercase(LangString &str1)
{
	for (unsigned int *c=(unsigned int *) str1.c_str(); *c; c++)
	{
		if (*c >= 'A' && *c <='Z') *c += 'a' - 'A';
	}
}

int LangStringUtil::strcmp(const LangString &str1, const LangString &str2)
{
	if (str1.size() < str2.size()) return -1;
	if (str1.size() > str2.size()) return 1;

	for (const unsigned int *c1 = str1.c_str(),
		*c2 = str2.c_str();
		*c1 && *c2;
		c1++, c2++)
	{
		if (c1 < c2) return -1;
		if (c1 > c2) return 1;
	}

	return 0;
	
	return (str1 == str2)?0:1;
}

int LangStringUtil::strcasecmp(const LangString &str1, const LangString &str2)
{
	LangString str1l(str1);
	LangString str2l(str2);
	lowercase(str1l);
	lowercase(str2l);
	return strcmp(str1l, str2l);
}

int LangStringUtil::strlen(const LangString &str1)
{
	return (int) str1.size();
}

unsigned int *LangStringUtil::strstr(const unsigned int *str1, const LangString &str2)
{
	LangString newString(str1);
	size_t pos = newString.find(str2);
	if (pos == LangString::npos) return 0;
	return (unsigned int*)(str1 + pos);
}

unsigned int *LangStringUtil::stristr(const unsigned int *str1, const LangString &str2)
{
	LangString str1l(str1);
	LangString str2l(str2);
	lowercase(str1l);
	lowercase(str2l);
	size_t pos = str1l.find(str2l);
	if (pos == LangString::npos) return 0;
	return (unsigned int*)(str1 + pos);
}

void LangStringUtil::trim(LangString &value)
{
	size_t start = value.find_first_not_of(LANG_STRING(" \t\n"));
	size_t end = value.find_last_not_of(LANG_STRING(" \t\n"));
	if (start == LangString::npos) value.clear();
	else if (end == LangString::npos) value.clear();
	else value = LangString(value, start, end-start+1);
}
