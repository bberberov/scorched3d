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

#if !defined(__INCLUDE_MetaClassh_INCLUDE__)
#define __INCLUDE_MetaClassh_INCLUDE__

#include <string>
#include <map>

#define REGISTER_CLASS_HEADER(x) \
\
class METAFACTORY_##x : public MetaClassFactory \
{ \
	virtual MetaClass *getClassCopy() { return new x ; } \
}; \
virtual const char *getClassName() { return #x ; } \
virtual unsigned int getMetaClassId() \
{ \
	static unsigned int metaClassID = nextMetaClassId_++; return metaClassID; \
} 

#define REGISTER_CLASS_SOURCE(x) \
	struct META_##x { META_##x() { MetaClassRegistration::addMap(#x , new x::METAFACTORY_##x ); } }; \
	static META_##x META_IMPL_##x ;

class MetaClass
{
public:
	MetaClass();
	virtual ~MetaClass();	

	// Automatically given by the 
	// REGISTER_CLASS_HEADER and
	// REGISTER_CLASS_SOURCE macros
	virtual unsigned int getMetaClassId() = 0;
	virtual const char *getClassName() = 0;
protected:
	static unsigned int nextMetaClassId_;
};

class MetaClassFactory
{
public:
	MetaClassFactory();
	virtual ~MetaClassFactory();
	
	// Automatically given by the 
	// REGISTER_CLASS_HEADER and
	// REGISTER_CLASS_SOURCE macros
	virtual MetaClass *getClassCopy() = 0;
};

class MetaClassRegistration
{
public:
	static void addMap(const char *name, MetaClassFactory *mclass);
	static std::map<std::string, MetaClassFactory *> *classMap;
	static MetaClass *getNewClass(const char *name);
	static MetaClassFactory *getFactory(const char *name);
};

#endif // __INCLUDE_MetaClassh_INCLUDE__
