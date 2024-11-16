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

#include <sprites/ExplosionLaserBeamRenderer.h>
#include <sprites/ExplosionTextures.h>
#include <common/Defines.h>
#include <console/Console.h>
#include <image/ImageFactory.h>
#include <sound/SoundUtils.h>
#include <client/ScorchedClient.h>
#include <graph/ParticleEmitter.h>
#include <graph/ParticleEngine.h>

REGISTER_CLASS_SOURCE(ExplosionLaserBeamRenderer);

GLTextureReference ExplosionLaserBeamRenderer::_texture(ImageID(S3D::eModLocation,
			"data/textures/waves.bmp",
			"data/textures/waves.bmp",
			false));

ExplosionLaserBeamRenderer::~ExplosionLaserBeamRenderer()
{
}

ExplosionLaserBeamRenderer::ExplosionLaserBeamRenderer():
	totalTime_(0), time_(0), size_(12.0f), angle_(0)
{

}

void ExplosionLaserBeamRenderer::init(unsigned int playerId,
	Vector &position, Vector &velocity, const char *data)
{
	if (0 != strcmp("none", data))
	{
		SoundBuffer *firedSound = 
			Sound::instance()->fetchOrCreateBuffer(
				S3D::getModFile(data));
		SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
			firedSound, position);
	}

	for (int j=0;j<layers;j++){
		for(int i=0;i<sides;i++){
			points[j][i]=Vector((float)(360/sides)*i,(double)((size_/(layers+1))*(j+1)));
		}
	}

	position_ = position;

	ParticleEmitter emmiter;
	emmiter.setAttributes(
			10.0f, 0.9f, // Life
			0.5f, 9.5f, // Mass
			0.0f, 0.0f, // Friction
			Vector::getNullVector(), Vector::getNullVector(), // Velocity
			Vector(0.9f, 0.9f, 0.1f), 0.9f, // StartColor1
			Vector(0.9f, 0.9f, 0.1f), 0.1f, // StartColor2
			Vector(0.6f, 0.6f, 0.95f), 0.0f, // EndColor1
			Vector(0.8f, 0.8f, 1.0f), 0.1f, // EndColor2
			0.0f, 0.0f, 0.5f, 0.5f, // Start Size
			0.0f, 0.0f, 10.0f, 10.0f, // EndSize
			Vector(0.0f, 0.0f, 10.0f), // Gravity
			true,
			false);

		Vector newPos1 = position_ + Vector(-4.0f, -4.0f, 0.0f);
		Vector newPos2 = position_ + Vector(4.0f, 4.0f, 0.0f);
		GLTextureSet *texture = ExplosionTextures::instance()->getTextureSetByName("particle");
		emmiter.emitLinear(800, newPos1, newPos2, 
			ScorchedClient::instance()->getParticleEngine(), 
			ParticleRendererQuads::getInstance(),
			texture,
			false);		
}

void ExplosionLaserBeamRenderer::draw(Action *action)
{
	GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);
	_texture.draw();

	glPushMatrix();	
	glTranslatef(position_[0],position_[1],0.0f);
	glScalef(time_*0.05f,time_*0.05f,1.0f);
	
	for (int j=0;j<layers;j++){
		glRotatef(angle_,0.0f,0.0f,1.0f);
		glBegin(GL_TRIANGLE_STRIP);
		int tempheight=(int)(time_*time_*time_);
		if (tempheight>100) tempheight=100;
		Vector height(0,0,tempheight);

		glColor4f(0.0f,0.6f,0.9f, 0.4f);
		
		for (int i=0;i<(sides+1);i++){
			
			glNormal3fv ((float*)(points[j][i%sides]));
			if (i%2){
				glTexCoord2f(0.0f, 0.0f+((layers-j)*time_));
			}else{
				glTexCoord2f(2.0f, 0.0f+((layers-j)*time_));
			}
			glVertex3fv((float*)(points[j][i%sides]+height));
			glNormal3fv ((float*)(points[j][i%sides]));
			if (i%2){
				glTexCoord2f(0.0f, (float)(tempheight/10)+((layers-j)*time_));
			}else{
				glTexCoord2f(2.0f, (float)(tempheight/10)+((layers-j)*time_));
			}
			glVertex3fv((float*)(points[j][i%sides]));
		}
		for (int i=0;i<(sides+1);i++){
			
			glNormal3fv ((float*)(points[j][i%sides]));
			if (i%2){
				glTexCoord2f(0.0f, 0.0f-((layers-j)*time_));
			}else{
				glTexCoord2f(2.0f, 0.0f-((layers-j)*time_));
			}
			glVertex3fv((float*)(points[j][i%sides]));
			glNormal3fv ((float*)(points[j][i%sides]));
			if (i%2){
				glTexCoord2f(0.0f, (float)(tempheight/10)-((layers-j)*time_));
			}else{
				glTexCoord2f(2.0f, (float)(tempheight/10)-((layers-j)*time_));
			}
			glVertex3fv((float*)(points[j][i%sides]+height));
		}
		
		glEnd();
	}
	glPopMatrix();
	
}
void ExplosionLaserBeamRenderer::simulate(Action *action, float frameTime, bool &remove)
{

	totalTime_ += frameTime;
	time_ += frameTime;
	angle_=(angle_+3.0f);
	if (angle_>360.0f){
		angle_=0.0f;
	}
	if ((time_)>size_){
		remove=true;
	}else{
		remove=false;
	}

		
}

