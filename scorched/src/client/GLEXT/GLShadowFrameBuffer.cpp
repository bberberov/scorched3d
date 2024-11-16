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

#include <GLEXT/GLShadowFrameBuffer.h>
#include <GLEXT/GLStateExtension.h>

GLShadowFrameBuffer::GLShadowFrameBuffer() : 
	frameBufferObject_(0), depthTextureObject_(0),
	width_(0), height_(0)
{
}

GLShadowFrameBuffer::~GLShadowFrameBuffer()
{
	destroy();
}

bool GLShadowFrameBuffer::create(int width, int height)
{
	width_ = width;
	height_ = height;

	// Create texture
	glGenTextures(1, &depthTextureObject_);
	glBindTexture(GL_TEXTURE_2D, depthTextureObject_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY); 

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, 
		GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);	

	// Create framebuffer
	glGenFramebuffersEXT(1, &frameBufferObject_);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObject_);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
		GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextureObject_, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, 
		GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthTextureObject_);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);  
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) 
	{
		return false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return true;
}

void GLShadowFrameBuffer::bindDepthTexture()
{
	glBindTexture(GL_TEXTURE_2D, depthTextureObject_);
	GLTexture::setLastBind(0);
}

void GLShadowFrameBuffer::destroy()
{
	if (!bufferValid()) return;

	//Activate the frame buffer objcet
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObject_);

	//Detach our depth texture from the frame buffer object
	glFramebufferTexture2DEXT(
		GL_FRAMEBUFFER_EXT,
		GL_DEPTH_ATTACHMENT_EXT,
		GL_TEXTURE_2D, 
		0,
		0);
	glFramebufferRenderbufferEXT(
		GL_FRAMEBUFFER_EXT, 
		GL_DEPTH_ATTACHMENT_EXT, 
		GL_RENDERBUFFER_EXT, 
		0);

	//DeActivate the frame buffer objcet
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//Delete the frame buffer object
	glDeleteFramebuffersEXT(1, &frameBufferObject_);

	frameBufferObject_ = 0;

	// Delete texture
	glDeleteTextures(1, &depthTextureObject_);
	depthTextureObject_ = 0;
}

void GLShadowFrameBuffer::bind()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObject_);
}

void GLShadowFrameBuffer::unBind()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
