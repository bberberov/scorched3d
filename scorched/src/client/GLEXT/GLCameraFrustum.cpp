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

#include <math.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLCameraFrustum.h>
#include <graph/OptionsDisplay.h>

Vector GLCameraFrustum::FrustrumRed(1.0f, 0.0f, 0.0f);
Vector GLCameraFrustum::FrustrumBlue(0.0f, 0.0f, 1.0f);
Vector GLCameraFrustum::FrustrumGreen(0.0f, 1.0f, 0.0f);
Vector GLCameraFrustum::FrustrumWhite(1.0f, 1.0f, 1.0f);

GLCameraFrustum *GLCameraFrustum::instance_ = 0;

GLCameraFrustum *GLCameraFrustum::instance()
{
	if (!instance_)
	{
		instance_ = new GLCameraFrustum;
	}

	return instance_;
}

GLCameraFrustum::GLCameraFrustum() :
	GameStateI("GLCameraFrustum")
{

}

GLCameraFrustum::~GLCameraFrustum()
{

}

void GLCameraFrustum::normalize(float vector[4])
{
   float fT = (float) sqrt(
	   vector[0] * vector[0] + 
	   vector[1] * vector[1] + 
	   vector[2] * vector[2]);

   vector[0] /= fT;
   vector[1] /= fT;
   vector[2] /= fT;
   vector[3] /= fT;
}

Vector &GLCameraFrustum::getBilboardVectorX()
{
	static Vector bil;
	bil[0] = s.fClip[0];
	bil[1] = s.fClip[4];
	bil[2] = s.fClip[8];
	bil *= s.aspect * 0.8f;

	return bil;
}

Vector &GLCameraFrustum::getBilboardVectorY()
{
	static Vector bil;
	bil[0] = s.fClip[1];
	bil[1] = s.fClip[5];
	bil[2] = s.fClip[9];
	bil *= 0.8f;

	return bil;
}

void GLCameraFrustum::draw(const unsigned state)
{
	// Get the current projection matrix from OpenGL
	// Get the current modelview matrix from OpenGL
	glGetFloatv(GL_PROJECTION_MATRIX, s.fProj);
	glGetFloatv(GL_MODELVIEW_MATRIX, s.fView);
	glGetFloatv(GL_VIEWPORT, s.viewport);
	s.aspect = s.viewport[2] / s.viewport[3];

	// Concenate the two matrices
	s.fClip[ 0] = s.fView[ 0] * s.fProj[ 0] + s.fView[ 1] * s.fProj[ 4] + s.fView[ 2] * s.fProj[ 8] + s.fView[ 3] * s.fProj[12];
	s.fClip[ 1] = s.fView[ 0] * s.fProj[ 1] + s.fView[ 1] * s.fProj[ 5] + s.fView[ 2] * s.fProj[ 9] + s.fView[ 3] * s.fProj[13];
	s.fClip[ 2] = s.fView[ 0] * s.fProj[ 2] + s.fView[ 1] * s.fProj[ 6] + s.fView[ 2] * s.fProj[10] + s.fView[ 3] * s.fProj[14];
	s.fClip[ 3] = s.fView[ 0] * s.fProj[ 3] + s.fView[ 1] * s.fProj[ 7] + s.fView[ 2] * s.fProj[11] + s.fView[ 3] * s.fProj[15];

	s.fClip[ 4] = s.fView[ 4] * s.fProj[ 0] + s.fView[ 5] * s.fProj[ 4] + s.fView[ 6] * s.fProj[ 8] + s.fView[ 7] * s.fProj[12];
	s.fClip[ 5] = s.fView[ 4] * s.fProj[ 1] + s.fView[ 5] * s.fProj[ 5] + s.fView[ 6] * s.fProj[ 9] + s.fView[ 7] * s.fProj[13];
	s.fClip[ 6] = s.fView[ 4] * s.fProj[ 2] + s.fView[ 5] * s.fProj[ 6] + s.fView[ 6] * s.fProj[10] + s.fView[ 7] * s.fProj[14];
	s.fClip[ 7] = s.fView[ 4] * s.fProj[ 3] + s.fView[ 5] * s.fProj[ 7] + s.fView[ 6] * s.fProj[11] + s.fView[ 7] * s.fProj[15];

	s.fClip[ 8] = s.fView[ 8] * s.fProj[ 0] + s.fView[ 9] * s.fProj[ 4] + s.fView[10] * s.fProj[ 8] + s.fView[11] * s.fProj[12];
	s.fClip[ 9] = s.fView[ 8] * s.fProj[ 1] + s.fView[ 9] * s.fProj[ 5] + s.fView[10] * s.fProj[ 9] + s.fView[11] * s.fProj[13];
	s.fClip[10] = s.fView[ 8] * s.fProj[ 2] + s.fView[ 9] * s.fProj[ 6] + s.fView[10] * s.fProj[10] + s.fView[11] * s.fProj[14];
	s.fClip[11] = s.fView[ 8] * s.fProj[ 3] + s.fView[ 9] * s.fProj[ 7] + s.fView[10] * s.fProj[11] + s.fView[11] * s.fProj[15];

	s.fClip[12] = s.fView[12] * s.fProj[ 0] + s.fView[13] * s.fProj[ 4] + s.fView[14] * s.fProj[ 8] + s.fView[15] * s.fProj[12];
	s.fClip[13] = s.fView[12] * s.fProj[ 1] + s.fView[13] * s.fProj[ 5] + s.fView[14] * s.fProj[ 9] + s.fView[15] * s.fProj[13];
	s.fClip[14] = s.fView[12] * s.fProj[ 2] + s.fView[13] * s.fProj[ 6] + s.fView[14] * s.fProj[10] + s.fView[15] * s.fProj[14];
	s.fClip[15] = s.fView[12] * s.fProj[ 3] + s.fView[13] * s.fProj[ 7] + s.fView[14] * s.fProj[11] + s.fView[15] * s.fProj[15];

	// Bilboard matrix
	s.fBilboard[0] = s.fClip[0];
	s.fBilboard[1] = s.fClip[4];
	s.fBilboard[2] = s.fClip[8];
	s.fBilboard[3] = 0.0f; // x

	s.fBilboard[4] = s.fClip[1];
	s.fBilboard[5] = s.fClip[5];
	s.fBilboard[6] = s.fClip[9];
	s.fBilboard[7] = 0.0f; // y

	s.fBilboard[8] = s.fClip[2];
	s.fBilboard[9] = s.fClip[6];
	s.fBilboard[10] = s.fClip[10];
	s.fBilboard[11] = 0.0f; // z

	s.fBilboard[12] = 0.0f;
	s.fBilboard[13] = 0.0f;
	s.fBilboard[14] = 0.0f;
	s.fBilboard[15] = 1.0f;

	// Extract the right plane
	s.frustum_[0][0] = s.fClip[ 3] - s.fClip[ 0];
	s.frustum_[0][1] = s.fClip[ 7] - s.fClip[ 4];
	s.frustum_[0][2] = s.fClip[11] - s.fClip[ 8];
	s.frustum_[0][3] = s.fClip[15] - s.fClip[12];
	normalize(s.frustum_[0]);

	// Extract the left plane
	s.frustum_[1][0] = s.fClip[ 3] + s.fClip[ 0];
	s.frustum_[1][1] = s.fClip[ 7] + s.fClip[ 4];
	s.frustum_[1][2] = s.fClip[11] + s.fClip[ 8];
	s.frustum_[1][3] = s.fClip[15] + s.fClip[12];
	normalize(s.frustum_[1]);

	// Extract the bottom plane
	s.frustum_[2][0] = s.fClip[ 3] + s.fClip[ 1];
	s.frustum_[2][1] = s.fClip[ 7] + s.fClip[ 5];
	s.frustum_[2][2] = s.fClip[11] + s.fClip[ 9];
	s.frustum_[2][3] = s.fClip[15] + s.fClip[13];
	normalize(s.frustum_[2]);

	// Extract the top plane
	s.frustum_[3][0] = s.fClip[ 3] - s.fClip[ 1];
	s.frustum_[3][1] = s.fClip[ 7] - s.fClip[ 5];
	s.frustum_[3][2] = s.fClip[11] - s.fClip[ 9];
	s.frustum_[3][3] = s.fClip[15] - s.fClip[13];
	normalize(s.frustum_[3]);

	// Extract the far plane
	s.frustum_[4][0] = s.fClip[ 3] - s.fClip[ 2];
	s.frustum_[4][1] = s.fClip[ 7] - s.fClip[ 6];
	s.frustum_[4][2] = s.fClip[11] - s.fClip[10];
	s.frustum_[4][3] = s.fClip[15] - s.fClip[14];
	normalize(s.frustum_[4]);

	// Extract the near plane
	s.frustum_[5][0] = s.fClip[ 3] + s.fClip[ 2];
	s.frustum_[5][1] = s.fClip[ 7] + s.fClip[ 6];
	s.frustum_[5][2] = s.fClip[11] + s.fClip[10];
	s.frustum_[5][3] = s.fClip[15] + s.fClip[14];
	normalize(s.frustum_[5]);
}

bool GLCameraFrustum::sphereInFrustum(Vector &point, 
	float fRadius, Vector &color)
{
	if (OptionsDisplay::instance()->getDrawBoundingSpheres())
	{
		static GLUquadric *obj = 0;
		if (!obj)
		{
			obj = gluNewQuadric();
			gluQuadricDrawStyle(obj, GLU_LINE);
		}

		GLState glState(GLState::TEXTURE_OFF);
		glColor3fv(color);
		glPushMatrix();
			glTranslatef(point[0], point[1], point[2]);
			gluSphere(obj, fRadius, 6, 6);
		glPopMatrix();
	}

	return sphereInFrustumThreadSafe(point, fRadius);
}

bool GLCameraFrustum::sphereInFrustumThreadSafe(Vector &point, float fRadius)
{
	for (int iCurPlane = 0; iCurPlane<6; iCurPlane++)
	{
		float value = 
			s.frustum_[iCurPlane][0] * point[0] + 
			s.frustum_[iCurPlane][1] * point[1] + 
			s.frustum_[iCurPlane][2] * point[2] + 
			s.frustum_[iCurPlane][3];
		if (value <= -fRadius)
		{
			return false;
		}
	}

	return true;
}

void GLCameraFrustum::backupFrustum()
{
	memcpy(&b, &s, sizeof(b));
}

void GLCameraFrustum::restoreFrustum()
{
	memcpy(&s, &b, sizeof(b));
}

void GLCameraFrustum::drawBilboard(Vector &position, Vector &color, float alpha,
	float width, float height, bool additive, int textureCoord)
{
	if (additive) glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	Vector &bilX = getBilboardVectorX();
	Vector &bilY = getBilboardVectorY();

	float bilXX = bilX[0] * width;
	float bilXY = bilX[1] * width;
	float bilXZ = bilX[2] * width;
	float bilYX = bilY[0] * height;
	float bilYY = bilY[1] * height;
	float bilYZ = bilY[2] * height;

	glColor4f(color[0], color[1], color[2], alpha);
	glBegin(GL_QUADS);
	switch(textureCoord)
	{
	default: glTexCoord2d(1.0f, 1.0f); break;
	case 1:  glTexCoord2d(0.0f, 1.0f); break;
	case 2:  glTexCoord2d(0.0f, 0.0f); break;
	case 3:  glTexCoord2d(1.0f, 0.0f); break;
	}
	glVertex3f(
		position[0] + bilXX + bilYX, 
		position[1] + bilXY + bilYY, 
		position[2] + bilXZ + bilYZ);
	switch(textureCoord)
	{
	default: glTexCoord2d(0.0f, 1.0f); break;
	case 1:  glTexCoord2d(0.0f, 0.0f); break;
	case 2:  glTexCoord2d(1.0f, 0.0f); break;
	case 3:  glTexCoord2d(1.0f, 1.0f); break;
	}
	glVertex3f(
		position[0] - bilXX + bilYX, 
		position[1] - bilXY + bilYY, 
		position[2] - bilXZ + bilYZ);
	switch(textureCoord)
	{
	default: glTexCoord2d(0.0f, 0.0f); break;
	case 1:  glTexCoord2d(1.0f, 0.0f); break;
	case 2:  glTexCoord2d(1.0f, 1.0f); break;
	case 3:  glTexCoord2d(0.0f, 1.0f); break;
	}
	glVertex3f(
		position[0] - bilXX - bilYX, 
		position[1] - bilXY - bilYY, 
		position[2] - bilXZ - bilYZ);
	switch(textureCoord)
	{
	default: glTexCoord2d(1.0f, 0.0f); break;
	case 1:  glTexCoord2d(1.0f, 1.0f); break;
	case 2:  glTexCoord2d(0.0f, 1.0f); break;
	case 3:  glTexCoord2d(0.0f, 0.0f); break;
	}
	glVertex3f(
		position[0] + bilXX - bilYX, 
		position[1] + bilXY - bilYY, 
		position[2] + bilXZ - bilYZ);
	glEnd();

	if (additive) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

