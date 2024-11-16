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

#include <image/Image.h>
#include <GLEXT/GLState.h>
#include <common/Vector.h>
#include <sky/Hemisphere.h>
#include <common/Defines.h>
#include <vector>
#include <math.h>

void Hemisphere::draw(float radius, float radius2, 
	int heightSlices, int rotationSlices,
	int startHeightSlice, int startRotationSlice,
	int endHeightSlice, int endRotationSlice,
	bool inverse, unsigned int flags)
{
	glBegin(GL_QUAD_STRIP);
	const float maxTexCoord = 1.0f;
	for (int j=startHeightSlice; j<endHeightSlice; j++) 
	{
		float theta1 = j * HALFPI / float(heightSlices);
		float theta2 = (j + 1) * HALFPI / float(heightSlices);

		for (int i=startRotationSlice;i<=endRotationSlice;i++) 
		{
			float theta3 = i * TWOPI / float(rotationSlices);
			float c = theta3 / TWOPI * maxTexCoord;

			Vector e1, p1;
			e1[0] = float(cos(theta1) * cos(theta3));
			e1[2] = float(sin(theta1));
			e1[1] = float(cos(theta1) * sin(theta3));
			p1[0] = radius * e1[0];
			p1[2] = radius2 * e1[2];
			p1[1] = radius * e1[1];

			Vector e2, p2;
			e2[0] = float(cos(theta2) * cos(theta3));
			e2[2] = float(sin(theta2));
			e2[1] = float(cos(theta2) * sin(theta3));
			p2[0] = radius * e2[0];
			p2[2] = radius2 * e2[2];
			p2[1] = radius * e2[1];
			
			if (!inverse)
			{
				if (flags & eWidthTexture) glTexCoord2f((p1[0] + radius) / (2 * radius),
					(p1[1] + radius) / (2 * radius));
				else glTexCoord2f(c, float(j) / float(heightSlices));
				glVertex3fv(p1);
				if (flags & eWidthTexture) glTexCoord2f((p2[0] + radius) / (2 * radius),
					(p2[1] + radius) / (2 * radius));
				else glTexCoord2f(c, float(j+1) / float(heightSlices));
				glVertex3fv(p2);
			}
			else
			{
				if (flags & eWidthTexture) glTexCoord2f((p2[0] + radius) / (2 * radius),
					(p2[1] + radius) / (2 * radius));
				else glTexCoord2f(c, float(j+1) / float(heightSlices));
				glVertex3fv(p2);
				if (flags & eWidthTexture) glTexCoord2f((p1[0] + radius) / (2 * radius),
					(p1[1] + radius) / (2 * radius));
				else glTexCoord2f(c, float(j) / float(heightSlices));
				glVertex3fv(p1);
			}
		}
		glEnd();
		glBegin(GL_QUAD_STRIP);
	}
	glEnd();
}

void Hemisphere::drawColored(float radius, float radius2, 
	int heightSlices, int rotationSlices,
	int startHeightSlice, int startRotationSlice,
	int endHeightSlice, int endRotationSlice,
	bool inverse, Image &colors, Vector &sunDir, int daytime,
	bool horizonGlow)
{
	GLubyte *bits = colors.getBits();

	glBegin(GL_QUAD_STRIP);
	const float maxTexCoord = 1.0f;
	for (int j=startHeightSlice; j<endHeightSlice; j++) 
	{
		float theta1 = j * HALFPI / float(heightSlices);
		if (j<0) theta1 *= 0.75f;
		float theta2 = (j + 1) * HALFPI / float(heightSlices);

		int colorJ = MAX(j - 1, 0);
		int colorIndexA = int(float(colorJ) / float(heightSlices) * 15.0f);
		int colorIndexB = int(float(j) / float(heightSlices) * 15.0f);
		int bitmapIndexA = daytime * colors.getComponents() + (16 * colors.getComponents()) * colorIndexA;
		int bitmapIndexB = daytime * colors.getComponents() + (16 * colors.getComponents()) * colorIndexB;

		DIALOG_ASSERT(bitmapIndexA >= 0);
		DIALOG_ASSERT(bitmapIndexB < 
			colors.getWidth() * colors.getHeight() * colors.getComponents());

		for (int i=startRotationSlice;i<=endRotationSlice;i++) 
		{
			float theta3 = i * TWOPI / float(rotationSlices);
			float c = theta3 / TWOPI * maxTexCoord;

			Vector e1, p1, c1;
			e1[0] = float(cos(theta1) * cos(theta3));
			e1[2] = float(sin(theta1));
			e1[1] = float(cos(theta1) * sin(theta3));
			p1[0] = radius * e1[0];
			p1[2] = radius2 * e1[2];
			p1[1] = radius * e1[1];

			{
				float dotP = 0.0f;
				if (horizonGlow)
				{
					dotP = e1.Normalize().dotP(sunDir);
					dotP = (dotP + 1.0f) / 4.0f;
				}

				c1[0] = MIN(float(bits[bitmapIndexA]) / 255.0f + dotP, 1.0f);
				if (colors.getComponents() == 1)
				{
					c1[1] = MIN(float(bits[bitmapIndexA]) / 255.0f + dotP, 1.0f);
					c1[2] = MIN(float(bits[bitmapIndexA]) / 255.0f + dotP, 1.0f);
				}
				else
				{
					c1[1] = MIN(float(bits[bitmapIndexA + 1]) / 255.0f + dotP, 1.0f);
					c1[2] = MIN(float(bits[bitmapIndexA + 2]) / 255.0f + dotP, 1.0f);
				}
			}

			Vector e2, p2, c2;
			e2[0] = float(cos(theta2) * cos(theta3));
			e2[2] = float(sin(theta2));
			e2[1] = float(cos(theta2) * sin(theta3));
			p2[0] = radius * e2[0];
			p2[2] = radius2 * e2[2];
			p2[1] = radius * e2[1];
			
			{
				float dotP = 0.0f;
				if (horizonGlow)
				{
					dotP = e2.Normalize().dotP(sunDir);
					dotP = (dotP + 1.0f) / 4.0f;
				}

				c2[0] = MIN(float(bits[bitmapIndexB]) / 255.0f + dotP, 1.0f);
				if (colors.getComponents() == 1)
				{
					c2[1] = MIN(float(bits[bitmapIndexB]) / 255.0f + dotP, 1.0f);
					c2[2] = MIN(float(bits[bitmapIndexB]) / 255.0f + dotP, 1.0f);
				}
				else
				{
					c2[1] = MIN(float(bits[bitmapIndexB + 1]) / 255.0f + dotP, 1.0f);
					c2[2] = MIN(float(bits[bitmapIndexB + 2]) / 255.0f + dotP, 1.0f);
				}
			}

			if (!inverse)
			{
				glColor3fv(c1);
				glVertex3fv(p1);
				glColor3fv(c2);
				glVertex3fv(p2);
			}
			else
			{
				glColor3fv(c2);
				glVertex3fv(p2);
				glColor3fv(c1);
				glVertex3fv(p1);
			}
		}
		glEnd();
		glBegin(GL_QUAD_STRIP);
	}
	glEnd();
}
