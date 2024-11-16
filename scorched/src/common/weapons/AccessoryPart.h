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

#if !defined(AFX_ACCESSORYPART_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
#define AFX_ACCESSORYPART_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_

#include <XML/XMLFile.h>
#include <net/NetBuffer.h>
#include <map>

#define REGISTER_ACCESSORY_HEADER(x, y) \
	virtual const char *getAccessoryTypeName() { return #x ; } \
	virtual AccessoryType getType() { return y ; } \
	virtual AccessoryPart *getAccessoryCopy() { return new x ; }

#define REGISTER_ACCESSORY_SOURCE(x) \
	struct META_##x { META_##x() { AccessoryMetaRegistration::addMap(#x , new x ); } }; \
	static META_##x META_IMPL_##x ;

class Accessory;
class AccessoryStore;
class AccessoryCreateContext;
class AccessoryPart  
{
public:
	enum AccessoryType
	{
		AccessoryWeapon,
		AccessoryParachute,
		AccessoryShield,
		AccessoryAutoDefense,
		AccessoryBattery
	};

	AccessoryPart();
	virtual ~AccessoryPart();

	void setParent(Accessory *parent) { parent_ = parent; }
	Accessory *getParent() { return parent_; }

	unsigned int getAccessoryPartId() { return accessoryPartId_; }
	static void resetAccessoryPartIds() { nextAccessoryPartId_ = 100000; }

	virtual bool parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode) = 0;
	virtual AccessoryType getType() = 0;
	virtual const char *getAccessoryTypeName() = 0;
	virtual AccessoryPart *getAccessoryCopy() = 0;

protected:
	static unsigned int nextAccessoryPartId_;
	unsigned int accessoryPartId_;
	Accessory *parent_;

};

class AccessoryMetaRegistration
{
public:
	static void addMap(const char *name, AccessoryPart *action);
	static AccessoryPart *getNewAccessory(const char *name, AccessoryStore *store);

private:
	static std::map<std::string, AccessoryPart *> *accessoryMap;
};

#endif // !defined(AFX_ACCESSORYPART_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
