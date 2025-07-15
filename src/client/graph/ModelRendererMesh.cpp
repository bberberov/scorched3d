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

#include <graph/ModelRendererMesh.h>
#include <graph/OptionsDisplay.h>
#include <3dsparse/ModelMaths.h>
#include <GLEXT/GLGlobalState.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLInfo.h>

ModelRendererMesh::ModelRendererMesh(Model *model) : 
	model_(model)
{
	setup();
}

ModelRendererMesh::~ModelRendererMesh()
{
	while (!boneTypes_.empty())
	{
		BoneType *type = boneTypes_.back();
		boneTypes_.pop_back();
		delete type;
	}
	while (!frameInfos_.empty())
	{
		MeshFrameInfo frameInfo = frameInfos_.back();
		frameInfos_.pop_back();
		if (frameInfo.displayList != 0)
		{
			glDeleteLists(frameInfo.displayList, 1);
		}
	}
}

void ModelRendererMesh::setup()
{
	std::vector<BoneType *> &baseTypes = model_->getBaseBoneTypes();
	std::vector<BoneType *>::iterator itor;
	for (itor = baseTypes.begin();
		itor != baseTypes.end();
		++itor)
	{
		boneTypes_.push_back(new BoneType(*(*itor)));
	}

	MeshFrameInfo frameInfo;
	for (int f=0; f<=model_->getTotalFrames(); f++)
	{
		frameInfos_.push_back(frameInfo);
	}

	for (unsigned int m=0; m<model_->getMeshes().size(); m++)
	{
		Mesh *mesh = model_->getMeshes()[m];
		if (mesh->getTexture().getData())
		{
			// Force loading of texture outside of display list
			mesh->getTexture().draw(true);
		}
	}
}

void ModelRendererMesh::drawBottomAligned(float currentFrame, 
	float distance, float fade, bool setState)
{
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -model_->getMin()[2].asFloat());
		draw(currentFrame, distance, fade, setState);
	glPopMatrix();
}

void ModelRendererMesh::draw(float currentFrame, 
	float distance, float fade, bool setState)
{
	drawModel(currentFrame, distance, fade, setState, model_->getMeshes(), 0);
}

Mesh *ModelRendererMesh::drawModel(float currentFrame, 
	float distance, float fade, bool setState, 
	std::vector<Mesh *> &meshes, Mesh *lastMesh)
{
	// Get the current frame for the animation
	// If we have no bones, when we only have one frame
	// Make sure the frame falls within the accepted bounds
	int frame = model_->getStartFrame();
	if (model_->getTotalFrames() > 1)
	{
		frame = ((unsigned int)(currentFrame)) % model_->getTotalFrames();
		if (frame < 0) frame = 0;
	}

	// Check settings
	bool useTextures = (!OptionsDisplay::instance()->getNoSkins() && model_->getTexturesUsed());
	bool vertexLighting = OptionsDisplay::instance()->getNoModelLighting();

	// Draw the model
	DIALOG_ASSERT(frame >= 0 && frame < (int) frameInfos_.size());
	unsigned int displayList = frameInfos_[frame].displayList;
	if (!displayList)
	{
		// Move the bones into position
		for (unsigned int b=0; b<boneTypes_.size(); b++)
		{
			Bone *bone = model_->getBones()[b];
			BoneType *type = boneTypes_[b];

			unsigned int posKeys = (unsigned int) bone->getPositionKeys().size();
			unsigned int rotKeys = (unsigned int) bone->getRotationKeys().size();
			if (posKeys == 0 && rotKeys == 0)
			{
				memcpy(type->final_, type->absolute_, sizeof(BoneMatrixType));
				continue;
			}

			BoneMatrixType m;
			bone->getRotationAtTime(fixed(frame), m);
					
			FixedVector &pos = bone->getPositionAtTime(fixed(frame));
			m[0][3] = pos[0];
			m[1][3] = pos[1];
			m[2][3] = pos[2];

			ModelMaths::concatTransforms(type->relative_, m, type->relativeFinal_);
			if (type->parent_ == -1)
			{
				memcpy(type->final_, type->relativeFinal_, sizeof(BoneMatrixType));
			}
			else
			{
				BoneType *parent = boneTypes_[type->parent_];
				ModelMaths::concatTransforms(parent->final_, type->relativeFinal_, type->final_);
			}
		}

		glNewList(displayList = glGenLists(1), GL_COMPILE);
			for (unsigned int m=0; m<meshes.size(); m++)
			{
				Mesh *mesh = meshes[m];
				drawMesh(mesh, lastMesh, frame, useTextures, vertexLighting);
				lastMesh = mesh;
			}
		glEndList();

		frameInfos_[frame].displayList = displayList;
	}

	unsigned int state = 0;
	if (setState)
	{
		state = GLState::BLEND_ON | GLState::ALPHATEST_ON | GLState::NORMALIZE_ON;
		if (useTextures)
		{
			state |= GLState::TEXTURE_ON;
		}
		else
		{
			state |= GLState::TEXTURE_OFF;
		}

		if (vertexLighting)
		{
			state |= GLState::LIGHTING_OFF | GLState::LIGHT1_OFF;
		}
		else
		{
			state |= GLState::LIGHTING_ON | GLState::LIGHT1_ON;
		}
	}
	GLGlobalState globalState(state);

	glCallList(displayList);
	GLInfo::addNoTriangles(model_->getNumberTriangles());

	GLTexture::setLastBind(0);

	return lastMesh;
}

void ModelRendererMesh::drawMesh(Mesh *mesh, Mesh *lastMesh, 
	int frame, bool useTextures, bool vertexLighting)
{
	if (useTextures)
	{
		if (mesh->getTexture().getData() &&
			(!lastMesh || mesh->getTexture().getData() != lastMesh->getTexture().getData()))
		{
			mesh->getTexture().draw(true);
		}
		if (mesh->getSphereMap())
		{
			glEnable(GL_TEXTURE_GEN_S);						
			glEnable(GL_TEXTURE_GEN_T);	
			glEnable(GL_TEXTURE_GEN_R);
			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}

		if (!lastMesh || lastMesh->getAmbientColor() != mesh->getAmbientColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientColor().asVector4());
		}
		if (!lastMesh || lastMesh->getDiffuseColor() != mesh->getDiffuseColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseColor().asVector4());
		}
		if (!lastMesh || lastMesh->getSpecularColor() != mesh->getSpecularColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularColor().asVector4());
		}
		if (!lastMesh || lastMesh->getEmissiveColor() != mesh->getEmissiveColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveColor().asVector4());
		}
		if (!lastMesh || lastMesh->getShininessColor() != mesh->getShininessColor())
		{
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor().asFloat());
		}
	}
	else
	{
		if (!lastMesh || lastMesh->getAmbientNoTexColor() != mesh->getAmbientNoTexColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mesh->getAmbientNoTexColor().asVector4());
		}
		if (!lastMesh || lastMesh->getDiffuseNoTexColor() != mesh->getDiffuseNoTexColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mesh->getDiffuseNoTexColor().asVector4());
		}
		if (!lastMesh || lastMesh->getSpecularNoTexColor() != mesh->getSpecularNoTexColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mesh->getSpecularNoTexColor().asVector4());
		}
		if (!lastMesh || lastMesh->getEmissiveNoTexColor() != mesh->getEmissiveNoTexColor())
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh->getEmissiveNoTexColor().asVector4());
		}
		if (!lastMesh || lastMesh->getShininessColor() != mesh->getShininessColor())
		{
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mesh->getShininessColor().asFloat());
		}
	}
	
	{
		drawVerts(mesh, vertexLighting, frame);
	}

	if (useTextures)
	{
		if (mesh->getSphereMap())
		{
			glDisable(GL_TEXTURE_GEN_S);						
			glDisable(GL_TEXTURE_GEN_T);	
			glDisable(GL_TEXTURE_GEN_R);
		}
	}
}

void ModelRendererMesh::drawVerts(Mesh *mesh, bool vertexLighting, int frame)
{
	// Draw the vertices
	FixedVector vec;
	glBegin(GL_TRIANGLES);

	int faceVerts[3];
	
	std::vector<Face *>::iterator itor;
	for (itor = mesh->getFaces().begin();
		itor != mesh->getFaces().end();
		++itor)
	{
		Face *face = *itor;

		for (int i=0; i<3; i++)
		{
			int index = face->v[i];
			faceVerts[i] = index;
		}
		
		if (faceVerts[0] != faceVerts[1] &&
			faceVerts[1] != faceVerts[2] &&
			faceVerts[0] != faceVerts[2])
		{
			for (int i=0; i<3; i++)
			{
				Vertex *vertex = mesh->getVertex(faceVerts[i]);

				if (vertexLighting)
				{
					if (GLState::getState() & GLState::TEXTURE_OFF) 
					{
						glColor3f(
							(mesh->getDiffuseNoTexColor()[0] * vertex->lightintense[0]).asFloat(),
							(mesh->getDiffuseNoTexColor()[1] * vertex->lightintense[1]).asFloat(),
							(mesh->getDiffuseNoTexColor()[2] * vertex->lightintense[2]).asFloat());
					}
					else
					{
						glColor3fv(vertex->lightintense.asVector());
					}
				}

				glTexCoord2f(face->tcoord[i][0].asFloat(), face->tcoord[i][1].asFloat());
				glNormal3fv(face->normal[i].asVector());

				if (vertex->boneIndex != -1)
				{
					BoneType *type = boneTypes_[vertex->boneIndex];

					// Note: Translation of MS to S3D coords
					FixedVector newPos, newVec;
					newPos[0] = vertex->position[0];
					newPos[1] = vertex->position[2];
					newPos[2] = vertex->position[1];

					ModelMaths::vectorRotate(newPos, type->final_, newVec);
					vec[0] = newVec[0];
					vec[1] = newVec[2];
					vec[2] = newVec[1];

					vec[0] += type->final_[0][3] + vertexTranslation_[0];
					vec[1] += type->final_[2][3] + vertexTranslation_[1];
					vec[2] += type->final_[1][3] + vertexTranslation_[2];
					
				}
				else
				{
					vec[0] = vertex->position[0] + vertexTranslation_[0];
					vec[1] = vertex->position[1] + vertexTranslation_[1];
					vec[2] = vertex->position[2] + vertexTranslation_[2];
				}

				glVertex3fv(vec.asVector());
			}
		}
	}
	glEnd();
}
