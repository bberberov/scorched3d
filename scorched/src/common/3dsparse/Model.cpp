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

#include <3dsparse/Model.h>
#include <3dsparse/ModelMaths.h>
#include <common/Defines.h>

Model::Model() : 
	startFrame_(0), totalFrames_(0), 
	noTriangles_(0),
	texturesUsed_(false)
{
}

Model::~Model()
{
	while (!meshes_.empty())
	{
		Mesh *mesh = meshes_.back();
		meshes_.pop_back();
		delete mesh;
	}
	while (!bones_.empty())
	{
		Bone *bone = bones_.back();
		bones_.pop_back();
		delete bone;
	}
	while (!baseBoneTypes_.empty())
	{
		BoneType *boneType = baseBoneTypes_.back();
		baseBoneTypes_.pop_back();
		delete boneType;
	}
}

void Model::centre()
{
	max_ = meshes_.front()->getMax();
	min_ = meshes_.front()->getMin();
	DIALOG_ASSERT(!meshes_.empty());

	std::vector<Mesh *>::iterator itor;
	for (itor = meshes_.begin();
		itor != meshes_.end();
		++itor)
	{
		max_[0] = MAX(max_[0], (*itor)->getMax()[0]);
		max_[1] = MAX(max_[1], (*itor)->getMax()[1]);
		max_[2] = MAX(max_[2], (*itor)->getMax()[2]);

		min_[0] = MIN(min_[0], (*itor)->getMin()[0]);
		min_[1] = MIN(min_[1], (*itor)->getMin()[1]);
		min_[2] = MIN(min_[2], (*itor)->getMin()[2]);
	}

	FixedVector centre = (max_ + min_) / -2;
	for (itor = meshes_.begin();
		itor != meshes_.end();
		++itor)
	{
		(*itor)->move(centre);
	}
	min_ += centre;
	max_ += centre;
}

void Model::setup()
{
	centre();
	setupBones();
	setupColor();
	countTextures();
}

void Model::countTextures()
{
	unsigned int texturesUsed = 0;
	std::vector<Mesh *>::iterator itor;
	for (itor = meshes_.begin();
		itor != meshes_.end();
		++itor)
	{
		Mesh *mesh = *itor;
		if (mesh->getTextureName()[0]) 
		{
			texturesUsed++;
		}
		noTriangles_ += (int) mesh->getFaces().size();
	}
	//DIALOG_ASSERT(texturesUsed == meshes_.size());
	texturesUsed_ = (texturesUsed > 0);
}

void Model::setupColor()
{
	FixedVector lightpos(fixed(true, -3000), fixed(true, -2000), 1);
	lightpos.StoreNormalize();

	std::vector<Mesh *>::iterator itor;
	for (itor = meshes_.begin();
		itor != meshes_.end();
		++itor)
	{
		Mesh *mesh = (*itor);
	
		std::vector<Face *>::iterator itor;
		for (itor = mesh->getFaces().begin();
			itor != mesh->getFaces().end();
			++itor)
		{
			Face *face = *itor;
			for (int i=0; i<3; i++)
			{
				Vertex *vertex = mesh->getVertex(face->v[i]);
				FixedVector &normal = face->normal[i];

				const fixed ambientLight = fixed(true, 2000);
				const fixed diffuseLight = fixed(true, 8000);
				fixed diffuseLightMult = 
					(((lightpos.dotP(normal.Normalize())) / 2) + fixed(true, 5000));
				fixed intense = diffuseLightMult * diffuseLight + ambientLight;
				if (intense > 1) intense = 1; 
				vertex->lightintense[0] = intense;
				vertex->lightintense[1] = intense;
				vertex->lightintense[2] = intense;
			}
		}
	}
}

void Model::setupBones()
{
	for (unsigned int b=0; b<bones_.size(); b++)
	{
		baseBoneTypes_.push_back(new BoneType());
	}

	for (unsigned int b=0; b<bones_.size(); b++)
	{
		Bone *bone = bones_[b];
		BoneType *type = baseBoneTypes_[b];

		// Find the parent bone
		for (unsigned int p=0; p<bones_.size(); p++)
		{
			Bone *parent = bones_[p];
			if (0 == strcmp(bone->getParentName(), parent->getName()))
			{
				type->parent_ = p;
				break;
			}
		}

		// Set rotation and position
		FixedVector rot;
		rot[0] = bone->getRotation()[0] * 180 / fixed::XPI;
		rot[1] = bone->getRotation()[1] * 180 / fixed::XPI;
		rot[2] = bone->getRotation()[2] * 180 / fixed::XPI;

		ModelMaths::angleMatrix(rot, type->relative_);
		type->relative_[0][3] = bone->getPosition()[0];
		type->relative_[1][3] = bone->getPosition()[1];
		type->relative_[2][3] = bone->getPosition()[2];

		// Setup child relations
		if (type->parent_ != -1)
		{
			BoneType *parent = baseBoneTypes_[type->parent_];

			ModelMaths::concatTransforms(parent->absolute_, type->relative_, type->absolute_);
			memcpy(type->final_, type->absolute_, sizeof(BoneMatrixType));
		}
		else // No parent
		{
			memcpy(type->absolute_, type->relative_, sizeof(BoneMatrixType));
			memcpy(type->final_, type->relative_, sizeof(BoneMatrixType));
		}
	}

	// Move vertexes to align with the bones
	bool referenceBones = false;
	for (unsigned int i=0; i<meshes_.size(); i++)
	{
		Mesh *mesh = meshes_[i];
		for (unsigned int j=0; j<mesh->getVertexes().size(); j++)
		{
			Vertex *vertex = mesh->getVertex(j);
			if (vertex->boneIndex != -1)
			{
				DIALOG_ASSERT(vertex->boneIndex < int(bones_.size()));

				BoneType *type = baseBoneTypes_[vertex->boneIndex];
				referenceBones = true;

				// Note: Translation of MS to S3D coords

				// Translation
				vertex->position[0] -= type->absolute_[0][3];
				vertex->position[1] -= type->absolute_[2][3];
				vertex->position[2] -= type->absolute_[1][3];

				// Rotation
				FixedVector tmpPos;
				tmpPos[0] = vertex->position[0];
				tmpPos[1] = vertex->position[2];
				tmpPos[2] = vertex->position[1];

				FixedVector tmp;
				ModelMaths::vectorIRotate(tmpPos, type->absolute_, tmp);
				vertex->position[0] = tmp[0];
				vertex->position[1] = tmp[2];
				vertex->position[2] = tmp[1];
			}
		}
	}

	if (!referenceBones) totalFrames_ = 1;
}
