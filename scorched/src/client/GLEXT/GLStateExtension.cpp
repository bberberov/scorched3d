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

#include <GLEXT/GLStateExtension.h>
#include <graph/OptionsDisplay.h>
#include <common/DefinesString.h>
#include <common/Logger.h>
#include <common/Defines.h>

bool GLStateExtension::hasHardwareShadows_ = false;
bool GLStateExtension::hasMultiTex_ = false;
bool GLStateExtension::hasCubeMap_ = false;
bool GLStateExtension::hasSphereMap_ = false;
bool GLStateExtension::hasVBO_ = false;
bool GLStateExtension::hasHardwareMipmaps_ = false;
bool GLStateExtension::envCombine_ = false;
bool GLStateExtension::noTexSubImage_ = false;
bool GLStateExtension::hasBlendColor_ = false;
bool GLStateExtension::hasShaders_ = false;
bool GLStateExtension::hasFBO_ = false;
bool GLStateExtension::hasDrawRangeElements_ = false;
bool GLStateExtension::isSoftwareOpenGL_ = false;
int GLStateExtension::textureUnits_ = 0;
int GLStateExtension::imageUnits_ = 0;
int GLStateExtension::textureCoords_ = 0;
int GLStateExtension::maxVarying_ = 0;
int GLStateExtension::maxElementVertices_ = 0;
int GLStateExtension::maxElementIndices_ = 0;
bool GLStateExtension::useSimpleShaders_ = true;

void GLStateExtension::setup()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		S3D::dialogExit("GLEW", (const char *) glewGetErrorString(err));
	}
	Logger::log(S3D::formatStringBuffer("GLEW VERSION:%s", glewGetString(GLEW_VERSION)));

	bool vertexRangeExceeded = false;
	if (!OptionsDisplay::instance()->getNoGLExt())
	{
		if (!OptionsDisplay::instance()->getNoGLDrawElements())
		{
			if (GLEW_EXT_draw_range_elements)
			{
				GLint maxElementIndices;
				glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxElementIndices);
				maxElementIndices_ = maxElementIndices;

				GLint maxElementVertices;
				glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxElementVertices);		 
				maxElementVertices_ = maxElementVertices;

				if (maxElementIndices_ < 16000 ||
					maxElementVertices_ < 128000)
				{
					vertexRangeExceeded = true;
				}
				else
				{
					hasDrawRangeElements_ = true;

					if (!OptionsDisplay::instance()->getNoVBO())
					{
						if (GLEW_ARB_vertex_buffer_object)
						{
							hasVBO_ = true;
						}
					}
				}
			}
		}

		if (!OptionsDisplay::instance()->getNoGLMultiTex())
		{
			if (GLEW_ARB_multitexture)
			{
				GLint textureUnits;
				glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureUnits);
				textureUnits_ = textureUnits;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &textureUnits);
				imageUnits_ = textureUnits;
				glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &textureUnits);
				textureCoords_ = textureUnits;

				hasMultiTex_ = true;
			}
		}

		if (!OptionsDisplay::instance()->getNoGLEnvCombine())
		{
			envCombine_ = (GLEW_ARB_texture_env_combine == GL_TRUE);
		}

		if (!OptionsDisplay::instance()->getNoGLCubeMap())
		{
			hasCubeMap_ = (GLEW_EXT_texture_cube_map == GL_TRUE ||
				GLEW_ARB_texture_cube_map == GL_TRUE);
		}

		if (!OptionsDisplay::instance()->getNoGLSphereMap())
		{
			hasSphereMap_ = true;
		}

		if (!OptionsDisplay::instance()->getNoGLHardwareMipmaps())
		{
			hasHardwareMipmaps_ = GLEW_SGIS_generate_mipmap == GL_TRUE;
		}

		if (GLEW_EXT_blend_color)
		{
			hasBlendColor_ = true;
		}

		{
			if (GLEW_EXT_framebuffer_object)
			{
				hasFBO_ = true;
			}
		}
		if (!OptionsDisplay::instance()->getNoGLShaders())
		{
			hasShaders_ = 
				GLEW_ARB_fragment_shader &&
				GLEW_ARB_shader_objects &&
				GLEW_ARB_vertex_shader &&
				(textureUnits_ >=4 );

			GLint textureUnits;
			glGetIntegerv(GL_MAX_VARYING_FLOATS_ARB, &textureUnits);
			maxVarying_ = textureUnits;
			if (!OptionsDisplay::instance()->getSimpleWaterShaders())
			{
				if (maxVarying_ > 32) useSimpleShaders_ = false;
			}
		}
		if (!OptionsDisplay::instance()->getNoGLShadows() &&
			hasShaders_)
		{
			if (GLEW_EXT_framebuffer_object &&
				GLEW_ARB_shadow &&
				GLEW_ARB_depth_texture &&
				GLEW_ARB_multitexture)
			{
				hasHardwareShadows_ = true;
			}
		}
	}
	if (strstr((const char *) glGetString(GL_RENDERER), "GDI Generic") != 0)
	{
		isSoftwareOpenGL_ = true;
	}

	noTexSubImage_ = OptionsDisplay::instance()->getNoGLTexSubImage();

	Logger::log(S3D::formatStringBuffer("GL_VENDOR:%s", glGetString(GL_VENDOR)));
	Logger::log(S3D::formatStringBuffer("GL_RENDERER:%s", glGetString(GL_RENDERER)));
	Logger::log(S3D::formatStringBuffer("GL_VERSION:%s", glGetString(GL_VERSION)));
	Logger::log(S3D::formatStringBuffer("GL_EXTENSIONS:%s", glGetString(GL_EXTENSIONS)));
	Logger::log(S3D::formatStringBuffer("TEXTURE UNITS: %s (%i tex units, %i image units, %i coords, %i varying)", 
		(hasMultiTex()?"On":"Off"),textureUnits_, imageUnits_, textureCoords_, getMaxVarying()));
	Logger::log(S3D::formatStringBuffer("VERTEX BUFFER OBJECT:%s", 
		(hasVBO()?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("DRAW RANGE ELEMENTS:%s%s", 
		(hasDrawRangeElements()?"On":"Off"), 
		(vertexRangeExceeded?" (DISABLED DUE TO MAX_ELEMENTS)":"")));
	Logger::log(S3D::formatStringBuffer("GL_MAX_ELEMENTS_VERTICES:%i", 
		getMaxElementVertices()));
	Logger::log(S3D::formatStringBuffer("GL_MAX_ELEMENTS_INDICES:%i", 
		getMaxElementIndices()));
	Logger::log(S3D::formatStringBuffer("FRAME BUFFER OBJECT:%s", 
		(hasFBO()?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("SHADERS:%s", 
		(!hasShaders_?"Off":"On")));
	Logger::log(S3D::formatStringBuffer("ENV COMBINE:%s", 
		(envCombine_?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("CUBE MAP:%s", 
		(hasCubeMap_?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("HW MIP MAPS:%s", 
		(hasHardwareMipmaps_?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("HW SHADOWS:%s", 
		(hasHardwareShadows_?"On":"Off")));
	Logger::log(S3D::formatStringBuffer("BLEND COLOR:%s", 
		(hasBlendColor_?"On":"Off")));
}
