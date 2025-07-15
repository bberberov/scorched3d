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

#include <3dsparse/Mesh.h>
#include <3dsparse/ModelStore.h>
#include <common/Defines.h>

Mesh::Mesh(const char *name) : name_(name),
	sphereMap_(false)
	
{
#ifndef S3D_SERVER
	textureSet_ = false;
#endif
}

Mesh::~Mesh()
{
	while (!faces_.empty())
	{
		Face *face = faces_.back();
		faces_.pop_back();
		delete face;
	}
	while (!vertexes_.empty())
	{
		Vertex *vertex = vertexes_.back();
		vertexes_.pop_back();
		delete vertex;
	}
}

void Mesh::insertVertex(Vertex &vertex)
{ 
	if (vertexes_.empty())
	{
		max_ = vertex.position;
		min_ = vertex.position;
	}
	else
	{
		max_[0] = MAX(max_[0], vertex.position[0]);
		max_[1] = MAX(max_[1], vertex.position[1]);
		max_[2] = MAX(max_[2], vertex.position[2]);

		min_[0] = MIN(min_[0], vertex.position[0]);
		min_[1] = MIN(min_[1], vertex.position[1]);
		min_[2] = MIN(min_[2], vertex.position[2]);
	}

	vertexes_.push_back(new Vertex(vertex)); 
}

void Mesh::move(FixedVector &movement)
{
	std::vector<Vertex *>::iterator itor;
	for (itor = vertexes_.begin();
		itor != vertexes_.end();
		++itor)
	{
		(*itor)->position += movement;
	}
	max_ += movement;
	min_ += movement;
}

void Mesh::setTextureName(const char *t)
{ 
	if (0 != strcmp(t, "none"))
	{
		sphereMap_ = (strstr(t, "/sphere_") != 0);
		textureName_ = t; 

#ifndef S3D_SERVER
		textureSet_ = true;
		texture_.setImageID(
			ImageID(S3D::eAbsLocation,
				textureName_, 
				aTextureName_));
#endif
	}
}

void  Mesh::setATextureName(const char *t) 
{
	aTextureName_ = t; 

#ifndef S3D_SERVER
	textureSet_ = true;
	texture_.setImageID(
		ImageID(S3D::eAbsLocation,
			textureName_, 
			aTextureName_));
#endif
}
