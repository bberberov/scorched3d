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

#if !defined(__INCLUDE_GLSLProgramh_INCLUDE__)
#define __INCLUDE_GLSLProgramh_INCLUDE__

#include <list>
#include <string>
#include <GLSL/GLSLShader.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLShadowFrameBuffer.h>
#include <common/Vector.h>

/// this class handles an OpenGL GLSL Program, that is a link unit of shaders.
///@note needs OpenGL 2.0
class GLSLProgram
{
public:
	GLSLProgram();
	~GLSLProgram();

	/// attach a shader
	void attach(GLSLShader &s);

	/// attach a shader
	void detach(GLSLShader &s);

	/// link program after all shaders are attached
	void link();

	/// use this program
	///@note link program before using it!
	void use() const;

	/// use fixed function pipeline instead of particular program
	static void use_fixed();

	/// set up texture for a particular shader name
	void set_gl_texture(GLTexture& tex, const char *texName, unsigned texunit) const;
	void set_gl_texture(GLShadowFrameBuffer &tex, const char *texName, unsigned texunit) const;
	void set_gl_texture_unit(const char *texname, unsigned texunit) const;

	/// set uniform variable
	void set_uniform(const char *name, const Vector& value) const;
	void set_uniform(const char *name, const float value) const;

	/// get vertex attribute index
	unsigned get_vertex_attrib_index(const char *name) const;

 protected:
	unsigned int id_;
	bool linked_;
	std::list<GLSLShader*> attached_shaders_;
	static const GLSLProgram* used_program_;

 private:
	GLSLProgram(const GLSLProgram&);
	GLSLProgram& operator= (const GLSLProgram&);
};

#endif // __INCLUDE_GLSLProgramh_INCLUDE__
