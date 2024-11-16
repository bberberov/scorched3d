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

#if !defined(__INCLUDE_ObjectGrouph_INCLUDE__)
#define __INCLUDE_ObjectGrouph_INCLUDE__

#include <map>
#include <string>
#include <common/DefinesAssert.h>

class ObjectGroupEntry;
class ObjectGroup
{
public:
	// Some stuff to allow objects to be removed during iteration
	// Also allows the objects to be kept in insertion ordering
	// so the ordering is consistent across platforms
	// but with fast removal and insertion speed
	class ObjectGroupEntryHolderIterator;
	class ObjectGroupEntryHolder
	{
	public:
		ObjectGroupEntryHolder(ObjectGroupEntryHolder *previous, ObjectGroupEntry *current)
		{
			this->previous = previous;
			this->current = current;
			this->next = 0;
			if (previous) previous->next = this;
		}

	protected:
		friend class ObjectGroup;
		friend class ObjectGroupEntryHolderIterator;

		ObjectGroupEntryHolder *next;
		ObjectGroupEntry *current;
		ObjectGroupEntryHolder *previous;
	};

	class ObjectGroupEntryHolderIterator
	{
	public :
		ObjectGroupEntryHolderIterator(ObjectGroup *group) : group_(group)
		{
			current_ = group->front_;
			DIALOG_ASSERT(!group_->iterator_);
			group_->iterator_ = this;
		}
		~ObjectGroupEntryHolderIterator()
		{
			group_->iterator_ = 0;
		}

		ObjectGroupEntry *getNext() 
		{
			if (!current_) return 0;
			ObjectGroupEntryHolder *tmp = current_;
			current_ = current_->next;
			return tmp->current;
		}

	protected:
		friend class ObjectGroup;
		ObjectGroup *group_;
		ObjectGroupEntryHolder *current_;
	};

	ObjectGroup(const char *name);
	virtual ~ObjectGroup();

	const char *getName() { return name_.c_str(); }

	ObjectGroupEntry *getObjectByPos(int position);
	bool hasObject(ObjectGroupEntry *object);

	void addObject(ObjectGroupEntry *object);
	bool removeObject(ObjectGroupEntry *object);

	int getObjectCount();

protected:
	friend class ObjectGroupEntryHolderIterator;

	std::string name_;
	ObjectGroupEntryHolder *front_;
	ObjectGroupEntryHolder *back_;
	ObjectGroupEntryHolderIterator *iterator_;
	std::map<ObjectGroupEntry *, ObjectGroupEntryHolder *> objects_;
};

#endif // __INCLUDE_ObjectGrouph_INCLUDE__
