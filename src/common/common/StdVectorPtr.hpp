////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_StdVectorPtr_hpp_INCLUDE__
#define __INCLUDE_StdVectorPtr_hpp_INCLUDE__

#include <vector>
#include <stdexcept>

// same as std::vector regarding the interface, but handles pointers.
template< class T > class StdVectorPtr
{
public:
	StdVectorPtr( size_t capacity = 0 ) : data( capacity ) {}

	~StdVectorPtr() { clear(); }

	void resize( size_t newsize )
	{
		if ( newsize < size() )
		{
			for ( size_t i = newsize; i < size(); ++i )
			{
				delete data[i];
				// set to zero, because if resize throws an exception,
				// objects could get destructed twice
				data[i] = 0;
			}
		}
		data.resize( newsize );
	}

	// clang-format off
	// uncrustify off
	size_t size() const     { return data.size(); }
	size_t capacity() const { return data.capacity(); }
	// uncrustify on
	// clang-format on

	void clear()
	{
		for ( size_t i = 0; i < size(); i++ )
		{
			delete data[i];
			// set to zero, because if clear throws an exception,
			// objects could get destructed twice
			data[i] = 0;
		}
		data.clear();
	}

	// exception safe, so first create space, then store
	void push_back( std::auto_ptr< T > ptr )
	{
		data.push_back( 0 );
		data.back() = ptr.release();
	}

	// clang-format off
	// uncrustify off
	T* const& operator[]( size_t n ) const { return data[n]; }
	T* const& at( size_t n ) const         { return data.at( n ); }
	// uncrustify on
	// clang-format on

	void reset( size_t n, T* ptr )
	{
		delete data[n];
		data[n] = ptr;
	}

	bool empty() const { return data.empty(); }

protected:
	std::vector< T* > data;

private:
	StdVectorPtr( const StdVectorPtr& );
	StdVectorPtr& operator=( const StdVectorPtr& );
};

#endif  // __INCLUDE_StdVectorPtr_hpp_INCLUDE__
