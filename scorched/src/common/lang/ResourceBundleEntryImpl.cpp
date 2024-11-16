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

#include <lang/ResourceBundleEntryImpl.h>
#include <common/DefinesAssert.h>
#include <common/DefinesString.h>

ResourceBundleEntryImpl::ResourceBundleEntryImpl(const std::string &key) :
	key_(key), parameterizedString_(LangString())
{

}

ResourceBundleEntryImpl::ResourceBundleEntryImpl(const std::string &key, 
	const LangString &value) :
	key_(key), parameterizedString_(value)
{
}

LangString ResourceBundleEntryImpl::getString()
{
	return parameterizedString_.getParameterizedString();
}

LangString ResourceBundleEntryImpl::getString(const LangStringConverter &param1)
{
	return parameterizedString_.getParameterizedString(param1);
}

LangString ResourceBundleEntryImpl::getString(const LangStringConverter &param1, const LangStringConverter &param2)
{
	return parameterizedString_.getParameterizedString(param1, param2);
}

LangString ResourceBundleEntryImpl::getString(const LangStringConverter &param1, 
	const LangStringConverter &param2, const LangStringConverter &param3)
{
	return parameterizedString_.getParameterizedString(param1, param2, param3);
}

LangString ResourceBundleEntryImpl::getString(const LangStringConverter &param1, 
	const LangStringConverter &param2, const LangStringConverter &param3, 
	const LangStringConverter &param4)
{
	return parameterizedString_.getParameterizedString(param1, param2, param3, param4);
}

LangString ResourceBundleEntryImpl::getString(const LangStringConverter &param1, 
	const LangStringConverter &param2, const LangStringConverter &param3, 
	const LangStringConverter &param4, const LangStringConverter &param5)
{
	return parameterizedString_.getParameterizedString(param1, param2, param3, param4, param5);
}

LangString ResourceBundleEntryImpl::getValue()
{
	return parameterizedString_.getValue();
}
