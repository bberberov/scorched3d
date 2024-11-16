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

#if !defined(__INCLUDE_TargetListh_INCLUDE__)
#define __INCLUDE_TargetListh_INCLUDE__

#include <memory.h>

class TargetList
{
public:
	TargetList() : objects_(0)
	{
		clear();
	}
	~TargetList()
	{
		clear();
	}

	void clear()
	{
		delete [] objects_;
		objects_ = 0;
		lastObject_ = 0;
		capacity_ = 0;
		count_ = 0;
	}

	bool empty() 
	{
		return (count_ == 0);
	}

	void reset() 
	{
		count_ = 0;
		lastObject_ = objects_;
	}

	void setCapacity(int capacity)
	{
		clear();
		capacity_ = capacity;
		objects_ = lastObject_ = new void*[capacity_];
		reset();
	}

	void **add(void *obj)
	{
		if (count_ == capacity_)
		{
			if (capacity_ == 0) capacity_ = 128;
			else capacity_ *= 2;

			void **newObjects = new void*[capacity_];
			memset(newObjects, 0, sizeof(void*) * capacity_);
			if (objects_)
			{
				memcpy(newObjects, objects_, sizeof(void *) * count_);
				delete [] objects_;
			}
			objects_ = newObjects;
			lastObject_ = &newObjects[count_];
		}

		*lastObject_ = obj;
		count_++;
		lastObject_++;

		return (lastObject_ - 1);
	}

	int getObjectCount() { return count_; }
	void **getObjects() { return objects_; }

private:
	int count_, capacity_;
	void **objects_, **lastObject_;
};

class TargetListIterator
{
public:
	TargetListIterator(): 
	  list_(0), i_(0), current_(0)
	{

	}

	TargetListIterator(TargetList &list)
	{
		init(list);
	}

	void init(TargetList &list)
	{
		list_ = &list;
		current_ = list_->getObjects();
		i_ = 0;
	}

	void *getNext()
	{
		if (i_ == list_->getObjectCount()) return 0;
		void *result = *current_;
		i_++;
		current_++;
		return result;
	}

private:
	int i_;
	void **current_;
	TargetList *list_;
};

#endif // __INCLUDE_TargetListh_INCLUDE__
