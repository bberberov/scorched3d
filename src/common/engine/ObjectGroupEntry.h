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

#if !defined(__INCLUDE_ObjectGroupEntryh_INCLUDE__)
#define __INCLUDE_ObjectGroupEntryh_INCLUDE__

#include <common/FixedVector.h>
#include <set>

class ObjectGroupEntryReference;
class ObjectGroup;
class ObjectGroups;
class NamedNetBuffer;
class NetBufferReader;
class ObjectGroupEntry
{
public:
	enum ObjectType {
		TypeTarget,
		TypeParticle
	};

	ObjectGroupEntry(ObjectGroups &groups);
	virtual ~ObjectGroupEntry();

	std::set<ObjectGroup *> &getAllGroups() { return containment_; }

	void removeFromAllGroups();

	virtual ObjectGroupEntry::ObjectType getType() = 0;
	virtual void *getObject() = 0;

	virtual FixedVector &getPosition() = 0;
	virtual FixedVector &getVelocity() = 0;
	virtual unsigned int getPlayerId() = 0;

	virtual bool writeMessage(NamedNetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

private:
	friend class ObjectGroup;
	friend class ObjectGroupEntryReference;

	void addToGroup(ObjectGroup *group);
	void removeFromGroup(ObjectGroup *group);

protected:
	ObjectGroups &groups_;
	std::set<ObjectGroup *> containment_;
	std::set<ObjectGroupEntryReference *> references_;
};

class ObjectGroupEntryReference
{
public:
	ObjectGroupEntryReference(ObjectGroupEntry *entry) : entry_(entry)
	{
		if (entry_) entry_->references_.insert(this);
	}

	virtual ~ObjectGroupEntryReference()
	{
		if (entry_) entry_->references_.erase(this);
		entry_ = 0;
	}

	ObjectGroupEntry *getEntry() 
	{ 
		return entry_;
	}

	void clearEntry()
	{
		entry_ = 0;
	}
protected:
	ObjectGroupEntry *entry_;
};

#endif // __INCLUDE_ObjectGroupEntryh_INCLUDE__
