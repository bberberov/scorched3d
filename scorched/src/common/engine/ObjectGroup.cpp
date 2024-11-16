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

#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <target/Target.h>
#include <common/Defines.h>

ObjectGroup::ObjectGroup(const char *name) :
	name_(name), front_(0), back_(0), iterator_(0)
{
}

ObjectGroup::~ObjectGroup()
{
	std::map<ObjectGroupEntry *, ObjectGroupEntryHolder *>::iterator itor;
	for (itor = objects_.begin();
		itor != objects_.end();
		itor++)
	{
		delete itor->second;
	}
	objects_.clear();
	front_ = 0;
	back_ = 0;
}

void ObjectGroup::addObject(ObjectGroupEntry *object)
{
	back_ = new ObjectGroupEntryHolder(back_, object);
	if (!front_) front_ = back_;

	object->addToGroup(this);
	objects_[object] = back_;
}

bool ObjectGroup::removeObject(ObjectGroupEntry *object)
{
	std::map<ObjectGroupEntry *, ObjectGroupEntryHolder *>::iterator itor =
		objects_.find(object);
	if (itor == objects_.end()) return false;

	ObjectGroupEntryHolder *holder = itor->second;
	if (holder->previous && holder->next)
	{
		holder->previous->next = holder->next;
		holder->next->previous = holder->previous;
	} 
	else if (!holder->previous && !holder->next)
	{
		front_ = 0;
		back_ = 0;
	}
	else if (holder->previous && !holder->next)
	{
		holder->previous->next = 0;
		back_ = holder->previous;
	}
	else if (!holder->previous && holder->next)
	{
		holder->next->previous = 0;
		front_ = holder->next;
	}

	if (iterator_ &&
		iterator_->current_ == holder)
	{
		iterator_->current_ = holder->next;
	}

	delete holder;
	object->removeFromGroup(this);
	objects_.erase(itor);

	return true;
}

ObjectGroupEntry *ObjectGroup::getObjectByPos(int position)
{
	int pos = position % int(objects_.size());
	ObjectGroupEntryHolderIterator iterator(this);
	ObjectGroupEntry *entry;
	while (entry = iterator.getNext())
	{
		if (pos-- <= 0) return entry;
	}
	return 0;
}

bool ObjectGroup::hasObject(ObjectGroupEntry *object)
{
	return (objects_.find(object) != objects_.end());
}

int ObjectGroup::getObjectCount()
{ 
	return (int) objects_.size(); 
}
