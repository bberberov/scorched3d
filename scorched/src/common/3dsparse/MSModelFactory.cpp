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

#include <3dsparse/MSModelFactory.h>
#include <common/Defines.h>

MSModelFactory::MSModelFactory() : lineNo_(0)
{
}

MSModelFactory::~MSModelFactory()
{
}

Model *MSModelFactory::createModel(const char *fileName)
{
	Model *model = new Model();

	FILE *in = fopen(fileName, "r");
	if (!in)
	{
		S3D::dialogExit("MSModelFactory", S3D::formatStringBuffer(
			"Failed to open MS model \"%s\"",
			fileName));
	}
	loadFile(in, fileName, model);
	model->setup();
	fclose(in);

	return model;
}

bool MSModelFactory::getNextLine(char *line, FILE *in)
{
	char * wincr; 
	while (fgets(line, 256, in) != 0)
	{
		lineNo_++;
		if ((wincr=strchr(line,'\r'))) 
		{ 
			*wincr='\n'; 
			*(wincr + 1) = '\0'; 
		} 

		if ((line[0] == '\\' && line[1] == '\\') ||
			(line[0] == '/' && line[1] == '/') ||
			(line[0] == '\0') || 
			(line[0] == '\n'))
		{
			// Skip this line
		}
		else return true;
	}

	return false;
}

void MSModelFactory::returnError(const char *fileName, const std::string &error)
{
	S3D::dialogExit("MSModelFactory", S3D::formatStringBuffer("%s in file %i:%s", 
		error.c_str(), lineNo_, fileName));
}

void MSModelFactory::loadFile(FILE *in, const char *fileName, Model *model)
{
	char filePath[256];
	snprintf(filePath, sizeof(filePath), "%s", fileName);
	char fixed1[20], fixed2[20], fixed3[20];
	char fixed4[20], fixed5[20], fixed6[20];

	char *sep;
	while ((sep=strchr(filePath, '\\'))) *sep = '/';
	sep = strrchr(filePath, '/');
	if (sep) *sep = '\0';

	char buffer[256];
	int frames = 0;
	if (!getNextLine(buffer, in)) returnError(fileName, "No frames");
	if (sscanf(buffer, "Frames: %i", &frames) != 1) 
		returnError(fileName, "Incorrect frames format");	
	model->setTotalFrames(frames);

	if (!getNextLine(buffer, in)) returnError(fileName, "No frame");
	if (sscanf(buffer, "Frame: %i", &frames) != 1) 
		returnError(fileName, "Incorrect frame format");	
	model->setStartFrame(frames);

	// Read number meshes
	int noMeshes = 0;
	if (!getNextLine(buffer, in)) returnError(fileName, "No meshes");
	if (sscanf(buffer, "Meshes: %i", &noMeshes) != 1) 
		returnError(fileName, "Incorrect meshes format");

	std::vector<int> meshMaterials;
	for (int i=0; i<noMeshes; i++)
	{
		// Read the mesh name, flags and material indices
		char meshName[256]; 
		int meshFlags, meshMatIndex;
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No mesh name");
		if (sscanf(buffer, "%s %i %i", meshName, &meshFlags, &meshMatIndex) != 3) 
			returnError(fileName, "Incorrect mesh name format");
		meshMaterials.push_back(meshMatIndex);
		
		// Create and add the new model
		Mesh *mesh = new Mesh(meshName);
		model->addMesh(mesh);

		// Read no vertices
		int noVertices = 0;
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No num vertices");
		if (sscanf(buffer, "%i", &noVertices) != 1) 
			returnError(fileName, "Incorrect num vertices format");

		int j;
		std::vector<FixedVector> tcoords;
		for (j=0; j<noVertices; j++)
		{
			// Read the current vertex
			int vertexFlags;
			FixedVector texCoord;
			Vertex vertex;
			if (!getNextLine(buffer, in)) 
				returnError(fileName, "No vertices");
			if (sscanf(buffer, "%i %s %s %s %s %s %i",
				&vertexFlags,
				fixed1, fixed2, fixed3,
				fixed4, fixed5, &vertex.boneIndex) != 7)
				returnError(fileName, "Incorrect vertices format");

			vertex.position[0] = fixed(fixed1);
			vertex.position[2] = fixed(fixed2);
			vertex.position[1] = fixed(fixed3);
			texCoord[0] = fixed(fixed4);
			texCoord[1] = fixed(1)-fixed(fixed5);

			tcoords.push_back(texCoord);
			mesh->insertVertex(vertex);
		}

		// Read no normals
		std::vector<FixedVector> normals;
		int noNormals = 0;
		if (!getNextLine(buffer, in)) 
				returnError(fileName, "No num normals");
		if (sscanf(buffer, "%i", &noNormals) != 1)
				returnError(fileName, "Incorrect num normals format");
		for (j=0; j<noNormals; j++)
		{
			// Read the current normal
			FixedVector normal;
			if (!getNextLine(buffer, in))
				returnError(fileName, "No normal");
			if (sscanf(buffer, "%s %s %s",
				fixed1, fixed2, fixed3) != 3)
				returnError(fileName, "Incorrect normal format");

			normal[0] = fixed(fixed1);
			normal[2] = fixed(fixed2);
			normal[1] = fixed(fixed3);

			normals.push_back(normal.Normalize());
		}

		// Read no faces
		int noFaces = 0;
		if (!getNextLine(buffer, in)) 
				returnError(fileName, "No num faces");
		if (sscanf(buffer, "%i", &noFaces) != 1) 
				returnError(fileName, "Incorrect num faces format");
		for (j=0; j<noFaces; j++)
		{
			// Read the current face
			int faceFlags, sGroup;
			int nIndex1, nIndex2, nIndex3;
			Face face;
			if (!getNextLine(buffer, in))
				returnError(fileName, "No face");
			if (sscanf(buffer, "%i %i %i %i %i %i %i %i",
				&faceFlags,
				&face.v[0], &face.v[2], &face.v[1],
				&nIndex1, &nIndex3, &nIndex2,
				&sGroup) != 8)
				returnError(fileName, "Incorrect face format");

			mesh->insertFace(face);
			DIALOG_ASSERT (nIndex1 < (int) normals.size());
			mesh->setFaceNormal(normals[nIndex1], j, 0);
			DIALOG_ASSERT (nIndex2 < (int) normals.size());
			mesh->setFaceNormal(normals[nIndex2], j, 1);
			DIALOG_ASSERT (nIndex3 < (int) normals.size());
			mesh->setFaceNormal(normals[nIndex3], j, 2);

			DIALOG_ASSERT (face.v[0] < (int) tcoords.size());
			mesh->setFaceTCoord(tcoords[face.v[0]], j, 0);
			DIALOG_ASSERT (face.v[1] < (int) tcoords.size());
			mesh->setFaceTCoord(tcoords[face.v[1]], j, 1);
			DIALOG_ASSERT (face.v[2] < (int) tcoords.size());
			mesh->setFaceTCoord(tcoords[face.v[2]], j, 2);
		}
	}

	// Read number materials
	int noMaterials = 0;
	if (!getNextLine(buffer, in))
		returnError(fileName, "No num materials");
	if (sscanf(buffer, "Materials: %i", &noMaterials) != 1)
		returnError(fileName, "Incorrect num materials format");

	for (int m=0; m<noMaterials; m++)
	{
		// material: name
		char materialName[256];
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No material name");
		if (sscanf(buffer, "%s", materialName) != 1)
			returnError(fileName, "Incorrect material name format");

		// ambient
		FixedVector4 ambient;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No material ambient");
		if (sscanf(buffer, "%s %s %s %s", 
			fixed1, fixed2, fixed3, fixed4) != 4) 
			returnError(fileName, "Incorrect material ambient format");
		ambient[0] = fixed(fixed1);
		ambient[1] = fixed(fixed2);
		ambient[2] = fixed(fixed3);
		ambient[3] = fixed(fixed4);

		// diffuse
		FixedVector4 diffuse;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No material diffuse");
		if (sscanf(buffer, "%s %s %s %s", 
			fixed1, fixed2, fixed3, fixed4) != 4)
			returnError(fileName, "Incorrect material diffuse format");
		diffuse[0] = fixed(fixed1);
		diffuse[1] = fixed(fixed2);
		diffuse[2] = fixed(fixed3);
		diffuse[3] = fixed(fixed4);

		// specular
		FixedVector4 specular;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No material specular");
		if (sscanf(buffer, "%s %s %s %s", 
			fixed1, fixed2, fixed3, fixed4) != 4)
			returnError(fileName, "Incorrect material specular format");
		specular[0] = fixed(fixed1);
		specular[1] = fixed(fixed2);
		specular[2] = fixed(fixed3);
		specular[3] = fixed(fixed4);

		// emissive
		FixedVector4 emissive;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No material emissive");
		if (sscanf(buffer, "%s %s %s %s", 
			fixed1, fixed2, fixed3, fixed4) != 4)
			returnError(fileName, "Incorrect material emissive format");
		emissive[0] = fixed(fixed1);
		emissive[1] = fixed(fixed2);
		emissive[2] = fixed(fixed3);
		emissive[3] = fixed(fixed4);

		// shininess
		fixed shininess;
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No material shininess");
		if (sscanf(buffer, "%s", fixed1) != 1)
			returnError(fileName, "Incorrect material shininess format");
		shininess = fixed(fixed1);

		// transparency
		fixed transparency;
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No material transparency");
		if (sscanf(buffer, "%s", &fixed1) != 1)
			returnError(fileName, "Incorrect material transparency format");
		transparency = fixed(fixed1);

		// color map
		char textureName[256];
		char fullTextureName[256];
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No material texture");
		if (sscanf(buffer, "%s", textureName) != 1)
			returnError(fileName, "No material texture format");
		textureName[strlen(textureName)-1] = '\0';
		snprintf(fullTextureName, 256, "%s/%s", filePath, &textureName[1]);
		while (sep=strchr(fullTextureName, '\\')) *sep = '/';

		// alphamap
		char textureNameAlpha[256];
		char fullTextureAlphaName[256];
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No material alpha texture");
		if (sscanf(buffer, "%s", textureNameAlpha) != 1)
			returnError(fileName, "No material alpha texture format");
		textureNameAlpha[strlen(textureNameAlpha)-1] = '\0';
		snprintf(fullTextureAlphaName, 256, "%s/%s", filePath, &textureNameAlpha[1]);
		while (sep=strchr(fullTextureAlphaName, '\\')) *sep = '/';

		// Assign this material to the appropriate meshes
		int modelIndex = 0;
		std::vector<Mesh *>::iterator mitor;
		for (mitor = model->getMeshes().begin();
			mitor != model->getMeshes().end();
			++mitor, modelIndex++)
		{
			if (meshMaterials[modelIndex] == m)
			{
				Mesh *mesh = *mitor;
				if (textureName[1]) // as the string starts with a "
				{
					mesh->setTextureName(fullTextureName);
					if (!S3D::fileExists(fullTextureName))
					{
						returnError(fileName, 
							S3D::formatStringBuffer("Failed to find texture \"%s\"",
								fullTextureName));
					}
				}
				if (textureNameAlpha[1])
				{
					mesh->setATextureName(fullTextureAlphaName);
					if (!S3D::fileExists(fullTextureAlphaName))
					{
						returnError(fileName,
							S3D::formatStringBuffer("Failed to find alpha texture \"%s\"",
								fullTextureAlphaName));
					}
				}
				mesh->getDiffuseColor() = diffuse;
				mesh->getAmbientColor() = ambient;
				mesh->getSpecularColor() = specular;
				mesh->getEmissiveColor() = emissive;
				mesh->getShininessColor() = shininess;
				mesh->getDiffuseNoTexColor() = diffuse;
				mesh->getAmbientNoTexColor() = ambient;
				mesh->getSpecularNoTexColor() = specular;
				mesh->getEmissiveNoTexColor() = emissive;
			}
		}
	}

	// Setup meshes with no materials
	int modelIndex = 0;
	std::vector<Mesh *>::iterator mitor;
	for (mitor = model->getMeshes().begin();
		mitor != model->getMeshes().end();
		++mitor, modelIndex++)
	{
		int materialIndex = meshMaterials[modelIndex];
		if (materialIndex == -1)
		{
			Mesh *mesh = *mitor;

			FixedVector4 ambientColor(fixed(true, 3000), fixed(true, 3000), fixed(true, 3000), 1);
			FixedVector4 diffuseColor(fixed(true, 8000), fixed(true, 8000), fixed(true, 8000), 1);
			mesh->getAmbientColor() = ambientColor;
			mesh->getDiffuseColor() = diffuseColor;
			mesh->getEmissiveColor() = FixedVector4::getNullVector();
			mesh->getSpecularColor() = FixedVector4::getNullVector();

			mesh->getAmbientNoTexColor() = ambientColor;
			mesh->getDiffuseNoTexColor() = diffuseColor;
			mesh->getEmissiveNoTexColor() = FixedVector4::getNullVector();
			mesh->getSpecularNoTexColor() = FixedVector4::getNullVector();

			mesh->getShininessColor() = 0;
		}
	}


	////////////////
	// The bone's position and rotation are left in the milkshape native xyz coord
	// system and are not read in translated into the S3D system.
	// The translation is done during the bone calculation at runtime
	///////////////

	// Read number bones
	int noBones = 0;
	if (!getNextLine(buffer, in))
		returnError(fileName, "No num bones");
	if (sscanf(buffer, "Bones: %i", &noBones) != 1)
		returnError(fileName, "Incorrect num bones format");

	for (int b=0; b<noBones; b++)
	{
		// bone: name
		char boneName[256];
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No bone name");
		if (sscanf(buffer, "%s", boneName) != 1)
			returnError(fileName, "Incorrect bone name format");

		// bone parent
		char boneParentName[256];
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No bone parent name");
		if (sscanf(buffer, "%s", boneParentName) != 1)
			returnError(fileName, "Incorrect bone parent name format");

		// flags, position, rotation
		int boneFlags;
		FixedVector bonePos, boneRot;
		if (!getNextLine(buffer, in)) 
			returnError(fileName, "No bone pos/rot");
		if (sscanf(buffer, "%i %s %s %s %s %s %s",
			&boneFlags,
			fixed1, fixed2, fixed3,
			fixed4, fixed5, fixed6) != 7)
			returnError(fileName, "Incorrect bone pos/rot format");
		bonePos[0] = fixed(fixed1);
		bonePos[1] = fixed(fixed2);
		bonePos[2] = fixed(fixed3);
		boneRot[0] = fixed(fixed4);
		boneRot[1] = fixed(fixed5);
		boneRot[2] = fixed(fixed6);

		Bone *bone = new Bone(boneName);
		bone->setParentName(boneParentName);
		bone->setPosition(bonePos);
		bone->setRotation(boneRot);

		// position key
		int noPositionKeys = 0;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No bone position keys");
		if (sscanf(buffer, "%i", &noPositionKeys) != 1)
			returnError(fileName, "Incorrect bone position keys format");

		for (int p=0; p<noPositionKeys; p++)
		{
			fixed time;
			FixedVector position;
			if (!getNextLine(buffer, in)) 
				returnError(fileName, "No bone position key");
			if (sscanf(buffer, "%s %s %s %s",
				fixed1,
				fixed2, fixed3, fixed4) != 4)
				returnError(fileName, "Incorrect bone position key");

			time = fixed(fixed1);
			position[0] = fixed(fixed2);
			position[1] = fixed(fixed3);
			position[2] = fixed(fixed4);

			BonePositionKey *key = new BonePositionKey(time, position);
			bone->addPositionKey(key);
		}

		// rotation key
		int noRotationKeys = 0;
		if (!getNextLine(buffer, in))
			returnError(fileName, "No bone rotation keys");
		if (sscanf(buffer, "%i", &noRotationKeys) != 1)
			returnError(fileName, "Incorrect bone rotation keys format");

		for (int r=0; r<noRotationKeys; r++)
		{
			fixed time;
			FixedVector rotation;
			if (!getNextLine(buffer, in)) 
				returnError(fileName, "No bone position key");
			if (sscanf(buffer, "%s %s %s %s",
				fixed1,
				fixed2, fixed3, fixed4) != 4)
				returnError(fileName, "Incorrect bone position key");
			time = fixed(fixed1);
			rotation[0] = fixed(fixed2);
			rotation[1] = fixed(fixed3);
			rotation[2] = fixed(fixed4);

			BoneRotationKey *key = new BoneRotationKey(time, rotation);
			bone->addRotationKey(key);
		}

		// Add bone
		model->addBone(bone);
	}
}
