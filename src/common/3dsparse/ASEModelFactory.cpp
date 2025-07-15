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

#include <3dsparse/ASEModelFactory.h>
#include <common/Defines.h>
#include <stdio.h>
#include <math.h>

extern FILE *asein;
extern int aseparse(void);
extern int aselineno;

static ASEModelFactory *factory_ = 0;
static Model *model_ = 0;

ASEModelFactory::ASEModelFactory()
{
}

ASEModelFactory::~ASEModelFactory()
{
}

Model *ASEModelFactory::createModel(const char *fileName,
	const char *texName)
{
	model_ = new Model();
	if (loadFile(fileName))
	{
		std::vector<Mesh *>::iterator itor;
		for (itor = model_->getMeshes().begin();
			itor != model_->getMeshes().end();
			++itor)
		{
			Mesh *mesh = *itor;
			mesh->setTextureName(texName);
		}
		model_->setup();
		calculateTexCoords();
	}
	else
	{
		delete model_;
		model_ = 0;
	}

	return model_;
}

bool ASEModelFactory::loadFile(const char *fileName)
{
	asein = fopen(fileName, "r");
	if (!asein)
	{
		return false;
	}

	// Reset variables for next parser
	factory_ = this;
	aselineno = 0;
	return (aseparse() == 0);
}

ASEModelFactory *ASEModelFactory::getCurrent()
{
	return factory_;
}

void ASEModelFactory::addMesh(char *meshName)
{
	Mesh *mesh = new Mesh(meshName);
	model_->addMesh(mesh);
}

Mesh *ASEModelFactory::getCurrentMesh()
{
	DIALOG_ASSERT(!model_->getMeshes().empty());
	Mesh *mesh = model_->getMeshes().back();
	return mesh;
}

FixedVector ASEModelFactory::getTexCoord(FixedVector &tri, MaxMag mag, FixedVector &max, FixedVector &min)
{
	FixedVector newTri = tri;
	newTri -= min;
	newTri /= (max - min);

	switch(mag)
	{
	case MagX:
		newTri /= 2;
		newTri[0] = newTri[1] * 2;
		newTri[1] = newTri[2];
		break;
	case MagY:
		newTri /= 2;
		newTri[0] = newTri[0];
		newTri[1] = newTri[2] + fixed(true, 5000);
		break;
	case MagZ:
		newTri /= 2;
		newTri[0] += fixed(true, 5000);
		newTri[1] += fixed(true, 5000);
		break;
	}

	return newTri;
}

void ASEModelFactory::calculateTexCoords()
{
	std::vector<Mesh *>::iterator itor;
	for (itor = model_->getMeshes().begin();
		itor != model_->getMeshes().end();
		++itor)
	{
		Mesh *mesh = *itor;
		std::vector<Face*>::iterator fitor;
		for (fitor = mesh->getFaces().begin();
			fitor != mesh->getFaces().end();
			++fitor)
		{
			Face &face = *(*fitor);

			FixedVector &triA = mesh->getVertex(face.v[0])->position;
			FixedVector &triB = mesh->getVertex(face.v[1])->position;
			FixedVector &triC = mesh->getVertex(face.v[2])->position;

			MaxMag maxMag = MagZ;
			FixedVector faceNormal = (face.normal[0] + face.normal[1] + face.normal[2]).Normalize();
			if (faceNormal[0].abs() >= faceNormal[1].abs() &&
				faceNormal[0].abs() >= faceNormal[2].abs())
			{
				maxMag = MagX;
			}
			else if (faceNormal[1].abs() >= faceNormal[0].abs() &&
				faceNormal[1].abs() >= faceNormal[2].abs())
			{
				maxMag = MagY;
			}

			face.tcoord[0] = getTexCoord(triA, maxMag, model_->getMax(), model_->getMin());
			face.tcoord[1] = getTexCoord(triB, maxMag, model_->getMax(), model_->getMin());
			face.tcoord[2] = getTexCoord(triC, maxMag, model_->getMax(), model_->getMin());
		}
	}
}

