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

#if !defined(__INCLUDE_GLSLShaderSetuph_INCLUDE__)
#define __INCLUDE_GLSLShaderSetuph_INCLUDE__

#include <GLSL/GLSLShader.h>
#include <GLSL/GLSLProgram.h>

class GLSLShaderSetup
{
public:
	/// create shader setup of two shaders
	GLSLShaderSetup(
		const std::string &filename_vshader,
		const std::string &filename_fshader,
		const GLSLShader::defines_list& dl = GLSLShader::defines_list());
	~GLSLShaderSetup();

	/// use this setup
	void use() const { prog_.use(); }

	/// use fixed function pipeline instead of particular setup
	static void use_fixed() { GLSLProgram::use_fixed(); }

	/// set up texture for a particular shader name
	void set_gl_texture(GLTexture &tex, const char *texname, unsigned texunitnr) const 
	{
		prog_.set_gl_texture(tex, texname, texunitnr);
	}
	void set_gl_texture(GLShadowFrameBuffer &tex, const char *texname, unsigned texunitnr) const
	{
		prog_.set_gl_texture(tex, texname, texunitnr);
	}
	void set_gl_texture_unit(const char *texname, unsigned texunitnr) const
	{
		prog_.set_gl_texture_unit(texname, texunitnr);
	}

	/// set uniform variable
	void set_uniform(const char *name, const Vector& value) const 
	{
		prog_.set_uniform(name, value);
	}
	void set_uniform(const char *name, const float value) const
	{
		prog_.set_uniform(name, value);
	}

	/// get vertex attribute index
	unsigned get_vertex_attrib_index(const char *name) const 
	{
		return prog_.get_vertex_attrib_index(name);
	}

protected:
	GLSLShader vs_, fs_;
	GLSLProgram prog_;

private:
	GLSLShaderSetup(const GLSLShaderSetup&);
	GLSLShaderSetup& operator= (const GLSLShaderSetup&);
};

#endif // __INCLUDE_GLSLShaderSetuph_INCLUDE__
