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

#include <GLEXT/GLVertexBufferObject.h>
#include <GLEXT/GLStateExtension.h>
#include <common/DefinesAssert.h>
#include <common/Logger.h>

GLVertexBufferObject::GLVertexBufferObject(bool indexbuffer) :
	id_(0), size_(0), mapped_(false),
	target_(indexbuffer ? GL_ELEMENT_ARRAY_BUFFER_ARB : GL_ARRAY_BUFFER_ARB)
{
	DIALOG_ASSERT(GLStateExtension::hasVBO());

	glGenBuffersARB(1, &id_);
}

GLVertexBufferObject::~GLVertexBufferObject()
{
	if (mapped_) unmap();
	glDeleteBuffersARB(1, &id_);
}

void GLVertexBufferObject::init_data(unsigned size, const void* data, int usage)
{
	size_ = size;
	bind();
	glBufferDataARB(target_, size_, data, usage);
	unbind();
}

void GLVertexBufferObject::init_sub_data(unsigned offset, unsigned subsize, const void* data)
{
	bind();
	glBufferSubDataARB(target_, offset, subsize, data);
	unbind();
}

void GLVertexBufferObject::bind() const
{
	glBindBufferARB(target_, id_);
}

void GLVertexBufferObject::unbind() const
{
	glBindBufferARB(target_, 0);
}

void* GLVertexBufferObject::map(int access)
{
	DIALOG_ASSERT(!mapped_); //("vertex buffer object mapped twice");
	bind();
	void* addr = glMapBufferARB(target_, access);
	DIALOG_ASSERT(addr); // ("vertex buffer object mapping failed");
	mapped_ = true;

	return addr;
}

void GLVertexBufferObject::unmap()
{
	DIALOG_ASSERT(mapped_) //("vertex buffer object not mapped before unmap()");
	mapped_ = false;
	bind();

	if (glUnmapBufferARB(target_) != GL_TRUE) 
	{
		Logger::log("failed to unmap Vertex Buffer object, data invalid");
	}
}
