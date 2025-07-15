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

#if !defined(__INCLUDE_GLVertexBufferObjecth_INCLUDE__)
#define __INCLUDE_GLVertexBufferObjecth_INCLUDE__

#include <GLEXT/GLState.h>

class GLVertexBufferObject
{
public:
	///> create buffer. Tell the handler if you wish to store indices or other data.
	GLVertexBufferObject(bool indexbuffer = false);
	~GLVertexBufferObject();

	///> call ONCE after creation, to set data size and optionally data.
	///> pointer to data can be NULL to just (re)size the buffer.
	///> usage can be one of GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB,
	///> GL_STREAM_COPY_ARB, GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB,
	///> GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB,
	///> GL_DYNAMIC_COPY_ARB
	void init_data(unsigned size, const void* data, int usage);
	///> init/set sub data
	void init_sub_data(unsigned offset, unsigned subsize, const void* data);
	///> bind buffer
	void bind() const;
	///> unbind buffer
	void unbind() const;
	///> map buffer to address, access is one of GL_READ_ONLY_ARB, GL_WRITE_ONLY_ARB,
	///> GL_ERAD_WRITE_ARB.
	void* map(int access);
	///> get size of mapped space
	unsigned int get_map_size() const { return size_; }
	///> unmap buffer
	void unmap();

protected:
	GLuint id_;
	unsigned int size_;
	bool mapped_;
	int target_;
};

#endif // __INCLUDE_GLVertexBufferObjecth_INCLUDE__
