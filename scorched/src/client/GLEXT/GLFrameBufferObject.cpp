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

#include <GLEXT/GLFrameBufferObject.h>
#include <GLEXT/GLStateExtension.h>
#include <common/DefinesAssert.h>

GLFrameBufferObject::GLFrameBufferObject() : 
	frameBufferObject_(0), depthBufferObject_(0),
	texture_(0), bound_(false)
{
}

GLFrameBufferObject::~GLFrameBufferObject()
{
	destroy();
}

bool GLFrameBufferObject::create(GLTexture &texture, bool withDepth)
{
	if (!GLStateExtension::hasFBO()) {
		S3D::dialogExit("GLFrameBufferObject", "NO FBO");
	}

	texture_ = &texture;

	// create and bind depth buffer if requested
	if (withDepth) {
		glGenRenderbuffersEXT(1, &depthBufferObject_);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferObject_);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
			texture.getWidth(), texture.getHeight());
	}

	// create and bind FBO
	glGenFramebuffersEXT(1, &frameBufferObject_);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObject_);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, texture.getTexName(), 0);

	// attach depth buffer if requested
	if (withDepth) {
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_RENDERBUFFER_EXT, depthBufferObject_);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		destroy();
		S3D::dialogExit("GLFrameBufferObject", "FBO Init");
	}

	// unbind for now
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	return true;
}

void GLFrameBufferObject::destroy()
{
	if (frameBufferObject_) glDeleteFramebuffersEXT(1, &frameBufferObject_);
	if (depthBufferObject_) glDeleteRenderbuffersEXT(1, &depthBufferObject_);

	frameBufferObject_ = 0;
	depthBufferObject_ = 0;
}

void GLFrameBufferObject::bind()
{
	if (bound_) S3D::dialogExit("GLFrameBufferObject", "already bound");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferObject_);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, texture_->getWidth(), texture_->getHeight());
	bound_ = true;
}

void GLFrameBufferObject::unBind()
{
	if (!bound_) S3D::dialogExit("GLFrameBufferObject", "not bound");

	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	bound_ = false;
}
