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

#include <graph/ModelRendererTree.h>
#include <graph/OptionsDisplay.h>
#include <3dsparse/TreeModelFactory.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLGlobalState.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLInfo.h>

bool ModelRendererTree::skipPre_ = false;

// Pine
GLTextureReference ModelRendererTree::pineTextureA_(ImageID(S3D::eModLocation,
				"data/textures/pine2.bmp",
				"data/textures/pine2a.bmp",
				false));
GLTextureReference ModelRendererTree::pineTextureB_(ImageID(S3D::eModLocation,
				"data/textures/pine3.bmp",
				"data/textures/pine3a.bmp", 
				false));
GLuint ModelRendererTree::treePineList = 0;
GLuint ModelRendererTree::treePineSmallList = 0;
GLuint ModelRendererTree::treePineSnowList = 0;
GLuint ModelRendererTree::treePineSnowSmallList = 0;
GLuint ModelRendererTree::treePine2List = 0;
GLuint ModelRendererTree::treePine2SmallList = 0;
GLuint ModelRendererTree::treePine2SnowList = 0;
GLuint ModelRendererTree::treePine2SnowSmallList = 0;
GLuint ModelRendererTree::treePine3List = 0;
GLuint ModelRendererTree::treePine3SmallList = 0;
GLuint ModelRendererTree::treePine3SnowList = 0;
GLuint ModelRendererTree::treePine3SnowSmallList = 0;
GLuint ModelRendererTree::treePine4List = 0;
GLuint ModelRendererTree::treePine4SmallList = 0;
GLuint ModelRendererTree::treePine4SnowList = 0;
GLuint ModelRendererTree::treePine4SnowSmallList = 0;
GLuint ModelRendererTree::treePineBurntList = 0;
GLuint ModelRendererTree::treePineBurntSmallList = 0;
GLuint ModelRendererTree::treePineYellowList = 0;
GLuint ModelRendererTree::treePineYellowSmallList = 0;
GLuint ModelRendererTree::treePineLightList = 0;
GLuint ModelRendererTree::treePineLightSmallList = 0;

// Palm
GLTextureReference ModelRendererTree::palmTextureA_(ImageID(S3D::eModLocation,
				"data/textures/pine.bmp",
				"data/textures/pinea.bmp",
				false));
GLTextureReference ModelRendererTree::palmTextureB_(ImageID(S3D::eModLocation,
				"data/textures/palm2.bmp",
				"data/textures/palm2a.bmp",
				false));
GLuint ModelRendererTree::treePalmList = 0;
GLuint ModelRendererTree::treePalmSmallList = 0;
GLuint ModelRendererTree::treePalm2List = 0;
GLuint ModelRendererTree::treePalm2SmallList = 0;
GLuint ModelRendererTree::treePalm3List = 0;
GLuint ModelRendererTree::treePalm3SmallList = 0;
GLuint ModelRendererTree::treePalm4List = 0;
GLuint ModelRendererTree::treePalm4SmallList = 0;
GLuint ModelRendererTree::treePalmBList = 0;
GLuint ModelRendererTree::treePalmB2List = 0;
GLuint ModelRendererTree::treePalmB3List = 0;
GLuint ModelRendererTree::treePalmB4List = 0;
GLuint ModelRendererTree::treePalmB5List = 0;
GLuint ModelRendererTree::treePalmB6List = 0;
GLuint ModelRendererTree::treePalmB7List = 0;
GLuint ModelRendererTree::treePalmBurntList = 0;
GLuint ModelRendererTree::treePalmBurntSmallList = 0;

// Oak
GLTextureReference ModelRendererTree::oakTextureA_(ImageID(S3D::eModLocation,
				"data/textures/oak.bmp",
				"data/textures/oaka.bmp",
				false));
GLuint ModelRendererTree::treeOakList = 0;
GLuint ModelRendererTree::treeOakSmallList = 0;
GLuint ModelRendererTree::treeOak2List = 0;
GLuint ModelRendererTree::treeOak2SmallList = 0;
GLuint ModelRendererTree::treeOak3List = 0;
GLuint ModelRendererTree::treeOak3SmallList = 0;
GLuint ModelRendererTree::treeOak4List = 0;
GLuint ModelRendererTree::treeOak4SmallList = 0;


static void drawPineLevel(float texX, float texY,
		float width, float height, float lowheight, 
		float texWidth = 0.125f, float count = 5.0f, bool doubleSide = false,
		float angOffset = 0.0f)
{
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(texX, texY);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, height);
		for (float i=360.0f; i>=0.0f; i-=360.0f / count)
		{
			float diff = RAND * 40.0f - 20.0f;
			glTexCoord2f(
				texX + (sinf((i + angOffset)/180.0f * PI) * texWidth), 
				texY + (cosf((i + angOffset)/180.0f * PI) * texWidth));
			glNormal3f(
				sinf((i+diff)/180.0f * PI) * (height - lowheight),
				cosf((i+diff)/180.0f * PI) * (height - lowheight),
				width);
			glVertex3f(
				sinf(i/180.0f * PI) * width, 
				cosf(i/180.0f * PI) * width, 
				lowheight);
		}
	glEnd();

	if (doubleSide)
	{
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(texX, texY);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, height);
		for (float i=0.0f; i<=360.0f; i+=360.0f / count)
		{
			float diff = RAND * 40.0f - 20.0f;
			glTexCoord2f(
				texX + (sinf((i + angOffset)/180.0f * PI) * texWidth), 
				texY + (cosf((i + angOffset)/180.0f * PI) * texWidth));
			glNormal3f(
				-sinf((i+diff)/180.0f * PI) * (height - lowheight),
				-cosf((i+diff)/180.0f * PI) * (height - lowheight),
				-width);
			glVertex3f(
				sinf(i/180.0f * PI) * width, 
				cosf(i/180.0f * PI) * width, 
				lowheight);
		}
	glEnd();
	}
}

static void drawPineTrunc(float width, float height, float lowheight,
	float x = 0.875f, float y = 0.0f, float w = 0.125f, float h = 0.1f,
	float steps = 3.0f)
{
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(x , y);
		glNormal3f(0.0, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, height);
		for (float i=360.0f; i>=0.0f; i-=360.0f / steps)
		{
			glTexCoord2f(x + w*(float(int(i*5.0f)%360)/360.0f), y + h);
			glNormal3f(
				sinf(i/180.0f * PI),
				cosf(i/180.0f * PI),
				0.0f);
			glVertex3f(
				sinf(i/180.0f * PI) * width, 
				cosf(i/180.0f * PI) * width, 
				lowheight);
		}
	glEnd();
}

static void drawPalmTrunc(float width, float height, float count,
	float x = 0.0f, float y = 0.0f,
	float w = 1.0f, float h = 0.125f)
{
	bool tex = false;
	glBegin(GL_QUAD_STRIP);
		for (float i=360.0f; i>=0.0f; i-=360.0f / count)
		{
			glNormal3f(
				sinf(i/180.0f * PI), 
				cosf(i/180.0f * PI), 
				0.0f);
			if (tex) glTexCoord2f(x, y);
			else glTexCoord2f(x, y + h);
			glVertex3f(
				sinf(i/180.0f * PI) * (width - 0.1f), 
				cosf(i/180.0f * PI) * (width - 0.1f), 
				height);
			if (tex) glTexCoord2f(x + w, y);
			else glTexCoord2f(x + w, y + h);
			glVertex3f(
				sinf(i/180.0f * PI) * width, 
				cosf(i/180.0f * PI) * width, 
				0.0f);
			tex = !tex;
		}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(x + w, y);
		glNormal3f(0.0, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, height + 0.05f);
		for (float i=360.0f; i>=0.0f; i-=360.0f / count)
		{
			if (tex) glTexCoord2f(x + w, y);
			else glTexCoord2f(x + w, y + h);
			glNormal3f(
				sinf(i/180.0f * PI), 
				cosf(i/180.0f * PI), 
				0.0f);
			glVertex3f(
				sinf(i/180.0f * PI) * 0.03f, 
				cosf(i/180.0f * PI) * 0.03f, 
				height - 0.01f);
			tex = !tex;
		}
	glEnd();
}

static void drawPalmLevel(
		float width1, float w2, float height, float height2,
		float count, float texX, float texY)
{
	glBegin(GL_QUADS);
		for (float i=360.0f; i>=0.0f;)
		{
			float diff = 0.5f * RAND - 0.25f;
			float width2 = (w2 * RAND * 0.3f) + (0.7f * w2);
			
			Vector A1(
				sinf((i-15.0f)/180.0f * PI) * width1, 
				cosf((i-15.0f)/180.0f * PI) * width1, 
				height);
			Vector A2(
				sinf(i/180.0f * PI) * width1, 
				cosf(i/180.0f * PI) * width1, 
				height2);
			Vector A3(
				sinf(i/180.0f * PI) * width2, 
				cosf(i/180.0f * PI) * width2, 
				height2 + diff);
			Vector A4(
				sinf((i-15.0f)/180.0f * PI) * width2, 
				cosf((i-15.0f)/180.0f * PI) * width2, 
				height + diff);

			Vector AN = ((A3 - A4) * (A3 - A2));
			glNormal3fv(AN);
			glTexCoord2f(texX, texY);
			glVertex3fv(A1);
			glTexCoord2f(texX, texY + 0.123f);
			glVertex3fv(A2);
			glTexCoord2f(texX + 0.37f, texY + 0.123f);
			glVertex3fv(A3);
			glTexCoord2f(texX + 0.37f, texY);
			glVertex3fv(A4);

			Vector B1(
				sinf((i-15.0f)/180.0f * PI) * width2, 
				cosf((i-15.0f)/180.0f * PI) * width2, 
				height + diff);
			Vector B2(
				sinf(i/180.0f * PI) * width2, 
				cosf(i/180.0f * PI) * width2, 
				height2 + diff);
			Vector B3(
				sinf(i/180.0f * PI) * width1, 
				cosf(i/180.0f * PI) * width1, 
				height2);
			Vector B4(
				sinf((i-15.0f)/180.0f * PI) * width1, 
				cosf((i-15.0f)/180.0f * PI) * width1, 
				height);
			Vector BN = ((B1 - B3) * (B2 - B1));
			glNormal3fv(BN);
			glTexCoord2f(texX + 0.37f, texY);
			glVertex3fv(B1);
			glTexCoord2f(texX + 0.37f, texY + 0.123f);
			glVertex3fv(B2);
			glTexCoord2f(texX, texY + 0.123f);
			glVertex3fv(B3);
			glTexCoord2f(texX, texY);
			glVertex3fv(B4);

			Vector C1(
				sinf((i+15.0f)/180.0f * PI) * width2, 
				cosf((i+15.0f)/180.0f * PI) * width2, 
				height + diff);
			Vector C2(
				sinf(i/180.0f * PI) * width2, 
				cosf(i/180.0f * PI) * width2, 
				height2 + diff);
			Vector C3(
				sinf(i/180.0f * PI) * width1, 
				cosf(i/180.0f * PI) * width1, 
				height2);
			Vector C4(
				sinf((i+15.0f)/180.0f * PI) * width1, 
				cosf((i+15.0f)/180.0f * PI) * width1, 
				height);

			Vector CN = ((C2 - C1) * (C3 - C2));
			glNormal3fv(CN);
			glTexCoord2f(texX + 0.37f, texY);
			glVertex3fv(C1);
			glTexCoord2f(texX + 0.37f, texY + 0.123f);
			glVertex3fv(C2);
			glTexCoord2f(texX, texY + 0.123f);
			glVertex3fv(C3);
			glTexCoord2f(texX, texY);
			glVertex3fv(C4);

			Vector D1(
				sinf((i+15.0f)/180.0f * PI) * width1, 
				cosf((i+15.0f)/180.0f * PI) * width1, 
				height);
			Vector D2(
				sinf(i/180.0f * PI) * width1, 
				cosf(i/180.0f * PI) * width1, 
				height2);
			Vector D3(
				sinf(i/180.0f * PI) * width2, 
				cosf(i/180.0f * PI) * width2, 
				height2 + diff);
			Vector D4(
				sinf((i+15.0f)/180.0f * PI) * width2, 
				cosf((i+15.0f)/180.0f * PI) * width2, 
				height + diff);

			Vector DN = ((D4 - D3) * (D2 - D4));
			glNormal3fv(DN);
			glTexCoord2f(texX, texY);
			glVertex3fv(D1);
			glTexCoord2f(texX, texY + 0.123f);
			glVertex3fv(D2);
			glTexCoord2f(texX + 0.37f, texY + 0.123f);
			glVertex3fv(D3);
			glTexCoord2f(texX + 0.37f, texY);
			glVertex3fv(D4);

			i-= (360.0f / (count + (count-1) * RAND));
		}
	glEnd();

}

ModelRendererTree::ModelRendererTree(Model *model, ModelID &id) : 
	model_(model), treeType_(0)
{
	const char *modelName = id.getMeshName();
	const char *skinName = id.getSkinName();
	bool burnt = modelName[0] == 'B';
	bool snow = skinName[0] == 'S';

	TreeModelFactory::TreeType normalType, burntType;
	TreeModelFactory::getTypes(&modelName[2], snow, 
		normalType, burntType);
	treeType_ = (burnt?burntType:normalType);
}

ModelRendererTree::~ModelRendererTree()
{
}

void ModelRendererTree::draw(float currentFrame, 
	float distance, float fade, bool setState)
{
	drawBottomAligned(currentFrame, distance, fade, setState);
}

void ModelRendererTree::drawBottomAligned(float currentFrame, 
	float distance, float fade, bool setState)
{
	GLGlobalState globalState(0);
	if (!skipPre_) drawInternalPre(setState);
	drawInternal(distance, fade, setState);
}

void ModelRendererTree::drawInternalPre(bool setState)
{
	// Create the tree textures and models
	static bool listsCreated = false;
	if (!listsCreated)
	{
		listsCreated = true;

		glNewList(treePineList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.625f, 0.875f, 0.7f, 0.3f, 0.1f);
			drawPineLevel(0.375f, 0.875f, 0.5f, 0.7f, 0.2f);
			drawPineLevel(0.125f, 0.875f, 0.3f, 1.1f, 0.5f);
		glEndList();
		glNewList(treePineSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.625f, 0.875f, 0.7f, 1.1f, 0.1f);
		glEndList();

		glNewList(treePineSnowList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.625f, 0.625f, 0.7f, 0.3f, 0.1f);
			drawPineLevel(0.375f, 0.625f, 0.5f, 0.7f, 0.2f);
			drawPineLevel(0.125f, 0.625f, 0.3f, 1.1f, 0.5f);
		glEndList();
		glNewList(treePineSnowSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.625f, 0.625f, 0.7f, 1.1f, 0.1f);
		glEndList();

		glNewList(treePineYellowList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.625f, 0.375f, 0.7f, 0.3f, 0.1f);
			drawPineLevel(0.375f, 0.375f, 0.5f, 0.7f, 0.2f);
			drawPineLevel(0.125f, 0.375f, 0.3f, 1.1f, 0.5f);
		glEndList();
		glNewList(treePineYellowSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.625f, 0.375f, 0.7f, 1.1f, 0.1f);
		glEndList();

		glNewList(treePineLightList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.625f, 0.125f, 0.7f, 0.3f, 0.1f);
			drawPineLevel(0.375f, 0.125f, 0.5f, 0.7f, 0.2f);
			drawPineLevel(0.125f, 0.125f, 0.3f, 1.1f, 0.5f);
		glEndList();
		glNewList(treePineLightSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.625f, 0.125f, 0.7f, 1.1f, 0.1f);
		glEndList();

		glNewList(treePineBurntList = glGenLists(1), GL_COMPILE);
			glColor3f(0.3f, 0.3f, 0.3f);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.875f, 0.875f, 0.7f, 0.3f, 0.1f);
			drawPineLevel(0.875f, 0.875f, 0.5f, 0.7f, 0.2f);
			drawPineLevel(0.875f, 0.875f, 0.3f, 1.1f, 0.5f);
		glEndList();
		glNewList(treePineBurntSmallList = glGenLists(1), GL_COMPILE);
			glColor3f(0.3f, 0.3f, 0.3f);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.875f, 0.875f, 0.7f, 1.1f, 0.1f);
		glEndList();

		glNewList(treePine2List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.18f, 0.836f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.836f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.836f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine2SmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.18f, 0.836f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePine3List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.18f, 0.5f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.5f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.5f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine3SmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.18f, 0.5f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePine4List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.18f, 0.172f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.172f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.18f, 0.172f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine4SmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.18f, 0.172f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePine2SnowList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.52f, 0.836f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.836f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.836f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine2SnowSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.52f, 0.836f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePine3SnowList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.52f, 0.5f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.5f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.5f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine3SnowSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.52f, 0.5f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePine4SnowList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.1f, 1.1f, 0.0f);
			drawPineLevel(0.52f, 0.172f, 0.7f, 0.3f, 0.1f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.172f, 0.5f, 0.7f, 0.2f, 0.18f, 8.0f);
			drawPineLevel(0.52f, 0.172f, 0.3f, 1.1f, 0.5f, 0.18f, 8.0f);
		glEndList();
		glNewList(treePine4SnowSmallList = glGenLists(1), GL_COMPILE);
			drawPineLevel(0.52f, 0.172f, 0.7f, 1.1f, 0.1f, 0.18f, 8.0f);
		glEndList();

		glNewList(treePalmList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 5.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 7.0f, 0.0f, 0.365f);
		glEndList();
		glNewList(treePalmSmallList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 3.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 3.0f, 0.0f, 0.365f);
		glEndList();

		glNewList(treePalm2List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 5.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 7.0f, 0.39f, 0.365f);
		glEndList();
		glNewList(treePalm2SmallList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 3.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 3.0f, 0.39f, 0.365f);
		glEndList();

		glNewList(treePalm3List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 5.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 7.0f, 0.0f, 0.25f);
		glEndList();
		glNewList(treePalm3SmallList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 3.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 3.0f, 0.0f, 0.25f);
		glEndList();

		glNewList(treePalm4List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 5.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 7.0f, 0.39f, 0.25f);
		glEndList();
		glNewList(treePalm4SmallList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.7f, 3.0f);
			drawPalmLevel(0.0f, 0.6f, 0.6f, 0.8f, 3.0f, 0.39f, 0.25f);
		glEndList();

		glNewList(treePalmBList = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.164f, 0.836f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.164f, 0.836f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB2List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.5f, 0.836f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.5f, 0.836f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB3List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.836f, 0.836f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.836f, 0.836f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB4List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.164f, 0.5f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.164f, 0.5f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB5List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.5f, 0.5f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.5f, 0.5f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB6List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.164f, 0.164f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.164f, 0.164f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();
		glNewList(treePalmB7List = glGenLists(1), GL_COMPILE);
			drawPalmTrunc(0.07f, 0.5f, 5.0f, 0.664f, 0.0f, 0.172f, 0.656f);
			drawPineLevel(0.164f, 0.5f, 0.5f, 0.4f, 0.3f, 0.164f, 8.0f, true, 15.0f);
			drawPineLevel(0.164f, 0.5f, 0.4f, 0.5f, 0.4f, 0.164f, 8.0f, false);
		glEndList();

		glNewList(treePalmBurntList = glGenLists(1), GL_COMPILE);
			glColor3f(0.3f, 0.3f, 0.3f);
			drawPalmTrunc(0.07f, 0.7f, 5.0f);
		glEndList();
		glNewList(treePalmBurntSmallList = glGenLists(1), GL_COMPILE);
			glColor3f(0.3f, 0.3f, 0.3f);
			drawPalmTrunc(0.07f, 0.7f, 3.0f);
		glEndList();

		glNewList(treeOakList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.5f, 0.836f, 0.4f, 0.5f, 0.3f, 0.164f, 8.0f, false, 15.0f);
			drawPineLevel(0.164f, 0.836f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, false);
			drawPineLevel(0.5f, 0.836f, 0.2f, 0.8f, 0.6f, 0.164f, 8.0f, true, 15.0f);
		glEndList();
		glNewList(treeOakSmallList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.164f, 0.836f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, true);
		glEndList();	

		glNewList(treeOak2List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.164f, 0.5f, 0.4f, 0.5f, 0.3f, 0.164f, 8.0f, false, 15.0f);
			drawPineLevel(0.836f, 0.836f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, false);
			drawPineLevel(0.164f, 0.5f, 0.2f, 0.8f, 0.6f, 0.164f, 8.0f, true, 15.0f);
		glEndList();
		glNewList(treeOak2SmallList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.836f, 0.836f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, true);
		glEndList();	

		glNewList(treeOak3List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.836f, 0.5f, 0.4f, 0.5f, 0.3f, 0.164f, 8.0f, false, 15.0f);
			drawPineLevel(0.5f, 0.5f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, false);
			drawPineLevel(0.836f, 0.5f, 0.2f, 0.8f, 0.6f, 0.164f, 8.0f, true, 15.0f);
		glEndList();
		glNewList(treeOak3SmallList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.5f, 0.5f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, true);
		glEndList();	

		glNewList(treeOak4List = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.5f, 0.164f, 0.4f, 0.5f, 0.3f, 0.164f, 8.0f, false, 15.0f);
			drawPineLevel(0.164f, 0.164f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, false);
			drawPineLevel(0.5f, 0.164f, 0.2f, 0.8f, 0.6f, 0.164f, 8.0f, true, 15.0f);
		glEndList();
		glNewList(treeOak4SmallList = glGenLists(1), GL_COMPILE);
			drawPineTrunc(0.02f, 0.75f, 0.0f, 0.836f, 0.0f, 0.164f, 0.164f, 8.0f);
			drawPineLevel(0.164f, 0.164f, 0.3f, 0.6f, 0.5f, 0.164f, 8.0f, true);
		glEndList();	
	}

	unsigned int state = 0;
	if (setState)
	{
		state = GLState::TEXTURE_ON;
		bool vertexLighting = OptionsDisplay::instance()->getNoModelLighting();
		if (!vertexLighting)
		{
			state |= 
				GLState::NORMALIZE_ON | 
				GLState::LIGHTING_ON | 
				GLState::LIGHT1_ON;

			Vector4 ambientColor(0.4f, 0.4f, 0.4f, 1.0f);
			Vector4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			Vector4 specularColor(0.0f, 0.0f, 0.0f, 1.0f);
			Vector4 emissiveColor(0.0f, 0.0f, 0.0f, 1.0f);
			float shininess = 0.0f;
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		}
		else
		{
			state |= 
				GLState::NORMALIZE_OFF | 
				GLState::LIGHTING_OFF | 
				GLState::LIGHT1_OFF;

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
	GLGlobalState globalState(state);
}

void ModelRendererTree::drawInternal(float distance, float fade, bool setState)
{
	// Figure out which display list to call
	GLTexture *texture = 0;
	GLuint treeList = 0;
	GLuint smallTreeList = 0;
	switch(treeType_)
	{
	case TreeModelFactory::ePineNormal:
		texture = pineTextureA_.getTexture();
		treeList = treePineList;
		smallTreeList = treePineSmallList;
		break;
	case TreeModelFactory::ePine2:
		texture = pineTextureB_.getTexture();
		treeList = treePine2List;
		smallTreeList = treePine2SmallList;
		break;
	case TreeModelFactory::ePine3:
		texture = pineTextureB_.getTexture();
		treeList = treePine3List;
		smallTreeList = treePine3SmallList;
		break;
	case TreeModelFactory::ePine4:
		texture = pineTextureB_.getTexture();
		treeList = treePine4List;
		smallTreeList = treePine4SmallList;
		break;
	case TreeModelFactory::ePine2Snow:
		texture = pineTextureB_.getTexture();
		treeList = treePine2SnowList;
		smallTreeList = treePine2SnowSmallList;
		break;
	case TreeModelFactory::ePine3Snow:
		texture = pineTextureB_.getTexture();
		treeList = treePine3SnowList;
		smallTreeList = treePine3SnowSmallList;
		break;
	case TreeModelFactory::ePine4Snow:
		texture = pineTextureB_.getTexture();
		treeList = treePine4SnowList;
		smallTreeList = treePine4SnowSmallList;
		break;
	case TreeModelFactory::ePineBurnt:
		texture = pineTextureA_.getTexture();
		treeList = treePineBurntList;
		smallTreeList = treePineBurntSmallList;
		break;
	case TreeModelFactory::ePineSnow:
		texture = pineTextureA_.getTexture();
		treeList = treePineSnowList;
		smallTreeList = treePineSnowSmallList;
		break;
	case TreeModelFactory::ePineYellow:
		texture = pineTextureA_.getTexture();
		treeList = treePineYellowList;
		smallTreeList = treePineYellowSmallList;
		break;
	case TreeModelFactory::ePineLight:
		texture = pineTextureA_.getTexture();
		treeList = treePineLightList;
		smallTreeList = treePineLightSmallList;
		break;
	case TreeModelFactory::ePalmNormal:
		texture = palmTextureA_.getTexture();
		treeList = treePalmList;
		smallTreeList = treePalmSmallList;
		break;
	case TreeModelFactory::ePalm2:
		texture = palmTextureA_.getTexture();
		treeList = treePalm2List;
		smallTreeList = treePalm2SmallList;
		break;
	case TreeModelFactory::ePalm3:
		texture = palmTextureA_.getTexture();
		treeList = treePalm3List;
		smallTreeList = treePalm3SmallList;
		break;
	case TreeModelFactory::ePalm4:
		texture = palmTextureA_.getTexture();
		treeList = treePalm4List;
		smallTreeList = treePalm4SmallList;
		break;
	case TreeModelFactory::ePalmB:
		texture = palmTextureB_.getTexture();
		treeList = treePalmBList;
		smallTreeList = treePalmBList;
		break;
	case TreeModelFactory::ePalmB2:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB2List;
		smallTreeList = treePalmB2List;
		break;
	case TreeModelFactory::ePalmB3:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB3List;
		smallTreeList = treePalmB3List;
		break;
	case TreeModelFactory::ePalmB4:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB4List;
		smallTreeList = treePalmB4List;
		break;
	case TreeModelFactory::ePalmB5:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB5List;
		smallTreeList = treePalmB5List;
		break;
	case TreeModelFactory::ePalmB6:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB6List;
		smallTreeList = treePalmB6List;
		break;
	case TreeModelFactory::ePalmB7:
		texture = palmTextureB_.getTexture();
		treeList = treePalmB7List;
		smallTreeList = treePalmB7List;
		break;
	case TreeModelFactory::ePalmBurnt:
		texture = palmTextureA_.getTexture();
		treeList = treePalmBurntList;
		smallTreeList = treePalmBurntSmallList;
		break;
	case TreeModelFactory::eOak:
		texture = oakTextureA_.getTexture();
		treeList = treeOakList;
		smallTreeList = treeOakSmallList;
		break;
	case TreeModelFactory::eOak2:
		texture = oakTextureA_.getTexture();
		treeList = treeOak2List;
		smallTreeList = treeOak2SmallList;
		break;
	case TreeModelFactory::eOak3:
		texture = oakTextureA_.getTexture();
		treeList = treeOak3List;
		smallTreeList = treeOak3SmallList;
		break;
	case TreeModelFactory::eOak4:
		texture = oakTextureA_.getTexture();
		treeList = treeOak4List;
		smallTreeList = treeOak4SmallList;
		break;
	};

	DIALOG_ASSERT(texture && treeList && smallTreeList);
	if (setState)
	{
		texture->draw();
	}
	
	if (OptionsDisplay::instance()->getLowTreeDetail() || distance > 125.0f)
	{
		glCallList(smallTreeList);
		GLInfo::addNoTriangles(20);
	}
	else 
	{
		glCallList(treeList);
		GLInfo::addNoTriangles(10);
	}
}

