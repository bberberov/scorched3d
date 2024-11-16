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

#include <GLSL/GLSLShader.h>
#include <GLEXT/GLStateExtension.h>
#include <common/Defines.h>
#include <string>

GLSLShader::GLSLShader(const char *filename, Type stype,
					   const defines_list& dl) :
	id_(0)
{
	DIALOG_ASSERT(GLStateExtension::hasShaders());

	switch (stype) 
	{
	case VERTEX:
		id_ = glCreateShaderObjectARB(GL_VERTEX_SHADER);
		break;
	case FRAGMENT:
		id_ = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
		break;
	default:
		DIALOG_ASSERT(0); // ("invalid shader type");
	}

	DIALOG_ASSERT(id_); // runtime_error("can't create glsl shader");

	// the program as string
	std::string prg;

	// add defines to top of list for preprocessor
	for (defines_list::const_iterator it = dl.begin(); it != dl.end(); ++it) {
		prg += std::string("#define ") + *it + "\n";
	}

	// read lines.
	FILE *in = fopen(filename, "rb");
	if (!in) S3D::dialogExit("GLSLShader",
		S3D::formatStringBuffer("ERROR: Cannot find shader file \"%s\"", filename));

	char buffer[256];
	while (fgets(buffer, 256, in) != 0)
	{
		prg += buffer;
	}
	fclose(in);

	const char* prg_cstr = prg.c_str();
	glShaderSourceARB(id_, 1, &prg_cstr, 0);

	glCompileShaderARB(id_);

	GLint compiled = GL_FALSE;
	glGetObjectParameterivARB(id_, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
	if (compiled == GL_FALSE) 
	{
		// get compile log
		GLint maxlength = 0;
		glGetObjectParameterivARB(id_, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxlength);
		std::string logText(maxlength+1, ' ');
		GLsizei length = 0;
		glGetInfoLogARB(id_, maxlength, &length, &logText[0]);		
		
		S3D::dialogExit("GLSLShader", 
			S3D::formatStringBuffer("Shader compiling failed %s : %s", filename, logText.c_str()));
	}
}

GLSLShader::~GLSLShader()
{
	glDeleteObjectARB(id_);
}
