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
#include <tankgraph/MissileMesh.h>
#include <graph/ModelRenderer.h>
#include <graph/ModelRendererStore.h>
#include <3dsparse/Model.h>
#include <GLEXT/GLLenseFlare.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h> // For porting

MissileMesh::MissileMesh(ModelID &missile) : 
	innerScale_(1.0f),
	scale_(1.0f)
{
	model_ = ModelRendererStore::instance()->loadModel(missile);

	// Make sure the missile is not too large
	const float maxSize = 2.0f;
	Vector min = model_->getModel()->getMin().asVector();
	Vector max = model_->getModel()->getMax().asVector();
	float size = (max - min).Magnitude();
	if (size > maxSize) innerScale_ = 2.2f / size;

	// Add lense flares (if any)
	std::vector<Mesh *>::iterator itor;
	for (itor = model_->getModel()->getMeshes().begin();
		itor != model_->getModel()->getMeshes().end();
		++itor)
	{
		Mesh *mesh = (*itor);
		const char *name = mesh->getName();
		if (strstr(name, "\"Flare") == name ||
			strstr(name, "\"flare") == name)
		{
			FlareInfo info;

			// Find the center that the flare should be eminated from
			info.position = (mesh->getMax() + mesh->getMin()).asVector() / 2.0f;
			info.size = MAX(1.0f, (mesh->getMax() - mesh->getMin()).asVector().Magnitude());

			flares_.push_back(info);
		}
	}	
}

MissileMesh::~MissileMesh()
{
}

void MissileMesh::draw(Vector &position, Vector &direction, int flareType, float rotation, Vector &rotationAxis, float frame)
{
	// Figure out the opengl roation matrix from the direction
	// of the fired missile
	Vector dir = direction.Normalize();
	const float radToDeg = 180.0f / 3.14f;
	float angXYRad = 3.14f - atan2f(dir[0], dir[1]);
	float angYZRad = acosf(dir[2]);
	float angXYDeg = angXYRad * radToDeg;
	float angYZDeg = angYZRad * radToDeg;

	// Apply the matrix and render the missile
	float scale = innerScale_ * scale_;
	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		
		glRotatef(angXYDeg, 0.0f, 0.0f, 1.0f);
		glRotatef(angYZDeg, 1.0f, 0.0f, 0.0f);

		glRotatef(rotation, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
		glScalef(scale, scale, scale);
		model_->draw(frame, 0.0f, 1.0f, true);
	glPopMatrix();

	// Draw any lense flares associated with the missile
	std::list<FlareInfo>::iterator flareItor;
	for (flareItor =  flares_.begin();
		 flareItor != flares_.end();
		 ++flareItor)
	{
		FlareInfo info = (*flareItor);

		float newX = info.position[0];
		float newY = info.position[1];
		float newZ = info.position[2];

		float newX2 = 
			(newX * getFastCos(rotation)) - 
			(newY * getFastSin(rotation));
		float newY2 = 
			(newX * getFastSin(rotation)) + 
			(newY * getFastCos(rotation)); 
		float newZ2 = 
			newZ;

        float newX3 = 
			newX2;
		float newY3 = 
			(newY2 * getFastCos(angYZRad)) - 
			(newZ2 * getFastSin(angYZRad));
		float newZ3 = 
			(newY2 * getFastSin(angYZRad)) + 
			(newZ2 * getFastCos(angYZRad)); 

		float newX4 = 
			(newX3 * getFastCos(angXYRad)) - 
			(newY3 * getFastSin(angXYRad));
		float newY4 = 
			(newX3 * getFastSin(angXYRad)) + 
			(newY3 * getFastCos(angXYRad)); 
		float newZ4 = 
			newZ3;

		Vector newPos;
		newPos[0] = position[0] + newX4 * scale;
		newPos[1] = position[1] + newY4 * scale;
		newPos[2] = position[2] + newZ4 * scale;

		GLLenseFlare::instance()->draw(newPos, false, flareType, info.size);
	}
}

void MissileMesh::setScale(float scale)
{
	scale_=scale;
}
