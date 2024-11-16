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

#include <GLSL/GLSLProgram.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Logger.h>
#include <common/Defines.h>

const GLSLProgram* GLSLProgram::used_program_(0);

GLSLProgram::GLSLProgram() : 
	id_(0), linked_(false)
{
	DIALOG_ASSERT(GLStateExtension::hasShaders());

	id_ = glCreateProgramObjectARB();

	DIALOG_ASSERT(id_);// ("can't create glsl program");
}

GLSLProgram::~GLSLProgram()
{
	if (used_program_ == this) 
	{
		// rather use some kind of "bug!"-exception here
		Logger::log("warning: deleting bound glsl program!");
		use_fixed();
	}
	// if shaders are still attached, it is rather a bug...
	std::list<GLSLShader*>::iterator it;
	for (it = attached_shaders_.begin(); 
		it != attached_shaders_.end(); 
		++it)
	{
		glDetachObjectARB(id_, (*it)->getId());
	}

	glDeleteObjectARB(id_);
}

void GLSLProgram::attach(GLSLShader &s)
{
	glAttachObjectARB(id_, s.getId());
	attached_shaders_.push_front(&s);
	linked_ = false;
}

void GLSLProgram::detach(GLSLShader &s)
{
	glDetachObjectARB(id_, s.getId());

	std::list<GLSLShader*>::iterator it;
	for (it = attached_shaders_.begin(); 
		it != attached_shaders_.end(); )
	{
		if (*it == &s) 
		{
			glDetachObjectARB(id_, (*it)->getId());
			it = attached_shaders_.erase(it);
		} else 
		{
			++it;
		}
	}
	linked_ = false;
}

void GLSLProgram::link()
{
	glLinkProgramARB(id_);
	
	GLint waslinked = GL_FALSE;
	glGetObjectParameterivARB(id_, GL_OBJECT_LINK_STATUS_ARB, &waslinked);
	if (waslinked == GL_FALSE) 
	{
		// get link log
		GLint maxlength = 0;
		glGetObjectParameterivARB(id_, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxlength);
		std::string logText(maxlength+1, ' ');
		GLsizei length = 0;
		glGetInfoLogARB(id_, maxlength, &length, &logText[0]);		
		
		S3D::dialogExit("GLSLProgram", 
			S3D::formatStringBuffer("linking of program failed : %s", logText.c_str()));
	}
	
	linked_ = true;
}

void GLSLProgram::use() const
{
	DIALOG_ASSERT(linked_);
	glUseProgramObjectARB(id_);
	used_program_ = this;
}

void GLSLProgram::set_gl_texture(GLTexture &tex, const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocationARB(id_, texname);
	glActiveTextureARB(GL_TEXTURE0 + texunit);
	tex.draw(true);
	glUniform1iARB(uniloc, texunit);
}

void GLSLProgram::set_gl_texture(GLShadowFrameBuffer &tex, const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocationARB(id_, texname);
	glActiveTextureARB(GL_TEXTURE0 + texunit);
	tex.bindDepthTexture();
	glUniform1iARB(uniloc, texunit);
}

void GLSLProgram::set_gl_texture_unit(const char *texname, unsigned texunit) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint uniloc = glGetUniformLocationARB(id_, texname);
	glActiveTextureARB(GL_TEXTURE0 + texunit);
	glUniform1iARB(uniloc, texunit);
}

void GLSLProgram::set_uniform(const char *name, const Vector& value) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint loc = glGetUniformLocationARB(id_, name);
	glUniform3fARB(loc, value[0], value[1], value[2]);
}

void GLSLProgram::set_uniform(const char *name, const float value) const
{
	DIALOG_ASSERT(used_program_ == this);

	GLint loc = glGetUniformLocationARB(id_, name);
	glUniform1fARB(loc, value);
}

unsigned GLSLProgram::get_vertex_attrib_index(const char *name) const
{
	DIALOG_ASSERT(used_program_ == this);

	return glGetAttribLocationARB(id_, name);
}

void GLSLProgram::use_fixed()
{
	glUseProgramObjectARB(0);
	used_program_ = 0;
}
