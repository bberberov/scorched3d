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

#include <tankgraph/TargetRendererImpl.h>
#include <tankgraph/TargetParticleRenderer.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <target/TargetLife.h>
#include <client/ScorchedClient.h>
#include <graph/ParticleEngine.h>
#include <graph/ParticleTypes.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <sky/Hemisphere.h>
#include <weapons/ShieldRound.h>
#include <weapons/ShieldSquare.h>
#include <weapons/Accessory.h>
#include <actions/TargetFalling.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLCamera.h>
#include <land/VisibilityPatchGrid.h>

TargetRendererImpl::HighlightType TargetRendererImpl::highlightType_ = 
	TargetRendererImpl::eNoHighlight;

TargetRendererImpl::TargetRendererImpl(Target *target) : 
	target_(target),
	particleMade_(false), tree_(false), matrixCached_(false),
	posX_(0.0f), posY_(0.0f), posZ_(0.0f),
	currentVisibilityPatch_(0), patchEpoc_(-1)
{

}

TargetRendererImpl::~TargetRendererImpl()
{
	setMovedPatch(0);
}

void TargetRendererImpl::moved()
{
	if (VisibilityPatchGrid::instance()->getEpocNumber() == 0) return;

	TargetVisibilityPatch *newPatch = 0;
	if (target_->getVisible())
	{
		FixedVector &position = target_->getLife().getTargetPosition();
		newPatch = VisibilityPatchGrid::instance()->getTargetVisibilityPatch(
			position[0].asInt(), position[1].asInt());
	}
	setMovedPatch(newPatch);
	matrixCached_ = false;
}

void TargetRendererImpl::setMovedPatch(TargetVisibilityPatch *newPatch)
{
	float boundingSize = target_->getLife().getFloatBoundingSize();
	if (boundingSize > 32.0f) // Landscape square size
	{
		if (newPatch) TargetVisibilityPatch::addLargeTarget(target_);
		else TargetVisibilityPatch::removeLargeTarget(target_);

		return;
	}

	if (patchEpoc_ != VisibilityPatchGrid::instance()->getEpocNumber())
	{
		currentVisibilityPatch_ = 0;
		patchEpoc_ = VisibilityPatchGrid::instance()->getEpocNumber();
	}
	if (newPatch != currentVisibilityPatch_)
	{
		if (currentVisibilityPatch_)
		{
			if (!tree_)
			{
				currentVisibilityPatch_->removeTarget(target_);
				if (!target_->getTargetName().empty()) 
					currentVisibilityPatch_->removeTooltip(target_);
			}
			else 
			{
				currentVisibilityPatch_->removeTree(target_);
			}
		}
		if (newPatch)
		{
			if (!tree_)
			{
				newPatch->addTarget(target_);
				if (!target_->getTargetName().empty()) 
					newPatch->addTooltip(target_);
			}
			else 
			{
				newPatch->addTree(target_);
			}
		}
		currentVisibilityPatch_ = newPatch;
	}
}

bool TargetRendererImpl::getVisible()
{
	if (!currentVisibilityPatch_ || !currentVisibilityPatch_->getVisible()) return false;
	if (!target_->getVisible()) return false;

	return true;
}

void TargetRendererImpl::drawShield(float shieldHit, float totalTime)
{
	// Create the shield textures
	static GLTextureReference shieldtexture(ImageID(
		S3D::eModLocation,
		"data/textures/shield.bmp",
		"data/textures/shielda.bmp", 
		false));
	static GLTextureReference texture(ImageID(
		S3D::eModLocation, 
		"data/textures/bordershield/grid2.bmp",
		"data/textures/bordershield/grid2.bmp",
		false));
	static GLTextureReference texture2(ImageID(
		S3D::eModLocation,
		"data/textures/bordershield/grid22.bmp",
		"data/textures/bordershield/grid22.bmp",
		false));
	static GLTextureReference magtexture(ImageID(
		S3D::eModLocation,
		"data/textures/shield2.bmp",
		"data/textures/shield2.bmp", 
		false));
	static GLUquadric *obj = 0;
	if (!obj)
	{
		obj = gluNewQuadric();
		gluQuadricTexture(obj, GL_TRUE);
	}

	// Create the shield objects
	static unsigned int squareListNo = 0;
	static unsigned int smallListNo = 0;
	static unsigned int smallHalfListNo = 0;
	static unsigned int spiralListNo = 0;
	GLTexture magTexture;
	if (!smallListNo)
	{
		glNewList(smallListNo = glGenLists(1), GL_COMPILE);
			gluSphere(obj, 1.0f, 8, 8);
		glEndList();
		glNewList(squareListNo = glGenLists(1), GL_COMPILE);
			glBegin(GL_QUADS);		// Draw The Cube Using quads
				glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Top)
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Top)
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Quad (Top)
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 1.0f);	// Bottom Right Of The Quad (Top)

				glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 1.0f);	// Top Right Of The Quad (Bottom)
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 1.0f);	// Top Left Of The Quad (Bottom)
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Bottom)
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Bottom)

				glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Front)
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Front)
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Front)
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Front)

				glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,-1.0f,-1.0f);	// Top Right Of The Quad (Back)
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,-1.0f,-1.0f);	// Top Left Of The Quad (Back)
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f,-1.0f);	// Bottom Left Of The Quad (Back)
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f,-1.0f);	// Bottom Right Of The Quad (Back)

				glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Left)
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Left)

				glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Right)
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Right)
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Right)
			glEnd();
		glEndList();
		glNewList(smallHalfListNo = glGenLists(1), GL_COMPILE);
			Hemisphere::draw(1.0f, 1.0f, 10, 10, 6, 0, 10, 10, true);
			Hemisphere::draw(1.0f, 1.0f, 10, 10, 6, 0, 10, 10, false);
		glEndList();
		glNewList(spiralListNo = glGenLists(1), GL_COMPILE);
			float height = 0.0f;
			float width = 0.0f;
			float totalA = 5.0f * PI;
			float aInc = PI / 6.0f;
			glBegin(GL_QUAD_STRIP);
			for (float a=0.0f; a<totalA; a+=aInc)
			{
				height += 0.05f;
				width += 0.05f;
				float x = getFastSin(a) * width;
				float y = getFastCos(a) * width;
				float z = height;
				glTexCoord2f(a / totalA, 0.0f);
				glVertex3f(x, y, z);
				glTexCoord2f(a / totalA, 1.0f);
				glVertex3f(x, y, z - 0.4f);
			}
			glEnd();

			height = 0.0f;
			width = 0.0f;
			glBegin(GL_QUAD_STRIP);
			for (float a=0.0f; a<5.0f * PI; a+=PI/6.0f)
			{
				height += 0.05f;
				width += 0.05f;
				float x = getFastSin(a) * width;
				float y = getFastCos(a) * width;
				float z = height;
				glTexCoord2f(a / totalA, 0.0f);
				glVertex3f(x, y, z - 0.4f);
				glTexCoord2f(a / totalA, 1.0f);
				glVertex3f(x, y, z);
			}
			glEnd();
		glEndList();
	}

	// Draw the actual shield
	Accessory *accessory = target_->getShield().getGraphicalCurrentShield();
	if (!accessory) return;
	Shield *shield = (Shield *) accessory->getAction();

	GLState state(GLState::BLEND_ON | GLState::TEXTURE_ON); 
	Vector &position = target_->getLife().getFloatPosition();
	Vector &color = shield->getColor();

	if (shield->getRound())
	{
		ShieldRound *round = (ShieldRound *) shield;
		if (shield->getShieldType() == Shield::ShieldTypeRoundMag)
		{
			magtexture.draw();

			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glColor4f(color[0], color[1], color[2], 0.4f);
			glPushMatrix();
				glTranslatef(position[0], position[1], position[2] + 1.0f);
				float scale = round->getActualRadius().asFloat() / 3.0f;
				glScalef(scale, scale, scale);

				glRotatef(totalTime * 800.0f, 0.0f, 0.0f, 1.0f);
				glCallList(spiralListNo);
				glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
				glCallList(spiralListNo);
				glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
				glCallList(spiralListNo);
			glPopMatrix();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_TRUE);
		}
		else if (round->getHalfShield())
		{
			texture.draw();
			glPushMatrix();
				glColor4f(color[0], color[1], color[2], 0.5f + shieldHit);
				glTranslatef(position[0], position[1], position[2]);
				glScalef(
					round->getActualRadius().asFloat(), 
					round->getActualRadius().asFloat(), 
					round->getActualRadius().asFloat());
				glCallList(smallHalfListNo);
			glPopMatrix();
		}
		else
		{
			texture.draw();
			glPushMatrix();
				glColor4f(color[0], color[1], color[2], 0.5f + shieldHit);
				glTranslatef(position[0], position[1], position[2]);
				glScalef(
					round->getActualRadius().asFloat(), 
					round->getActualRadius().asFloat(), 
					round->getActualRadius().asFloat());
				glCallList(smallListNo);
			glPopMatrix();

			if (round->getGlow())
			{
				shieldtexture.draw();
				GLCameraFrustum::instance()->drawBilboard(
					position,
					color,
					1.0f - shieldHit,
					round->getActualRadius().asFloat() * 0.95f, 
					round->getActualRadius().asFloat() * 0.95f,
					true, // Additive
					0); // texcoord
			}
		}
	}
	else
	{
		ShieldSquare *square = (ShieldSquare *) shield;

		texture.draw();
		glPushMatrix();
			glColor4f(color[0], color[1], color[2], 0.5f + shieldHit);
			glTranslatef(position[0], position[1], position[2]);
			glScalef(
				square->getSize()[0].asFloat(), 
				square->getSize()[1].asFloat(), 
				square->getSize()[2].asFloat());
			glCallList(squareListNo);
		glPopMatrix();
	}
}

void TargetRendererImpl::drawParachute()
{
	static GLuint listNo = 0;
	if (!listNo)
	{
		float a;
		glNewList(listNo = glGenLists(1), GL_COMPILE);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);
				for (a=0.0f; a< 3.14f*2.0f; a+=3.14f / 4.0f)
				{
					glVertex3f(0.0f, 0.0f, 0.0f);
					glVertex3f(sinf(a) * 2.0f, cosf(a) * 2.0f, 2.0f);
				}
			glEnd();
			glBegin(GL_TRIANGLE_FAN);
				glVertex3f(0.0f, 0.0f, 3.0f);

				glColor3f(0.5f, 0.5f, 0.5f);
				for (a=3.14f*2.0f; a> 0.0f; a-=3.14f / 4.0f)
				{
					glVertex3f(sinf(a) * 2.0f, cosf(a) * 2.0f, 2.0f);
				}				
			glEnd();
		glEndList();
	}

	// Check this tank is falling
	if (!target_->getTargetState().getFalling()) return;

	// Check this tank has parachutes
	if (!target_->getTargetState().getFalling()->getParachute())
	{
		return;
	}

	Vector &position = target_->getLife().getFloatPosition();
	GLState state(GLState::TEXTURE_OFF);
	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glCallList(listNo);
	glPopMatrix();
}

void TargetRendererImpl::createParticle()
{
	// Check if we have made the particle
	// We may not have if there were not enough to create the 
	// tank in the first place
	if (particleMade_) return;

	// If this is a target we only need the particle
	// if we have a shield or if we are falling
	if (target_->getType() != Target::TypeTank)
	{
		if (!target_->getShield().getGraphicalCurrentShield() &&
			(tree_ || !target_->getTargetState().getFalling()))
		{
			return;
		}
	}

	// Else we need the particle
	{

		// Pretent the tank is actually a particle, this is so
		// it gets rendered during the particle renderering phase
		// and using the correct z ordering
		Particle *particle = 
			ScorchedClient::instance()->getParticleEngine().
				getNextAliveParticle(ParticleTarget);
		if (particle)
		{
			particle->setParticle(
				1000.0f,  1.0f, 1.0f, //float life, float mass, float friction,
				Vector::getNullVector(), Vector::getNullVector(), //Vector &velocity, Vector &gravity,
				Vector::getNullVector(), Vector::getNullVector(), //Vector &color, Vector &colorCounter,
				Vector::getNullVector(), Vector::getNullVector(), //Vector &size, Vector &sizeCounter,
				1.0f, 0.0f, // float alpha, float alphaCounter,
				false, //bool additiveTexture,
				false); //bool windAffect);

			particleMade_ = true;
			particle->life_ = 1000.0f;
			particle->renderer_ = TargetParticleRenderer::getInstance();
			particle->userData_ = new TargetParticleUserData(target_->getPlayerId());
		}
	}
}

float TargetRendererImpl::getTargetSize()
{
	// Target size
	float targetSize = target_->getLife().getFloatBoundingSize();
	float shieldSize = target_->getShield().getShieldBoundingSize().asFloat();
	float size = MAX(targetSize, shieldSize);
	return size;
}

float TargetRendererImpl::getTargetFade(float distance, float size)
{
	// Figure out the drawing distance
	float drawCullingDistance = OptionsDisplay::instance()->getDrawCullingDistance() * size;
	float drawFadeStartDistance =  OptionsDisplay::instance()->getDrawFadeStartDistance();
	float drawFadeDistance = drawCullingDistance - drawFadeStartDistance;
	float fade = 1.0f;
	if (distance > drawFadeStartDistance)
	{
		fade = 1.0f - ((distance - drawFadeStartDistance) / drawFadeDistance);
	}

	return fade;
}

void TargetRendererImpl::storeTarget2DPos()
{
	if (target_->getTargetName().empty()) return;

	Vector &tankTurretPos = 
		target_->getLife().getFloatCenterPosition();
	Vector camDir = 
		GLCamera::getCurrentCamera()->getLookAt() - 
		GLCamera::getCurrentCamera()->getCurrentPos();
	Vector tankDir = tankTurretPos - 
		GLCamera::getCurrentCamera()->getCurrentPos();

	if (camDir.dotP(tankDir) < 0.0f)
	{
		posX_ = - 1000.0;
	}
	else
	{
		static GLdouble modelMatrix[16];
		static GLdouble projMatrix[16];
		static GLint viewport[4];

		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		glGetIntegerv(GL_VIEWPORT, viewport);

		viewport[2] = GLViewPort::getWidth();
		viewport[3] = GLViewPort::getHeight();
		gluProject(
			tankTurretPos[0], 
			tankTurretPos[1], 
			tankTurretPos[2],
			modelMatrix, projMatrix, viewport,
			&posX_, 
			&posY_,
			&posZ_);
	}
}
