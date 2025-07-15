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

#include <common/ModelID.h>
#include <common/Defines.h>
#include <net/NetBuffer.h>
#include <XML/XMLParser.h>
#include <3dsparse/TreeModelFactory.h>

ModelID::ModelID()
{
}

ModelID::ModelID(const ModelID &other) : 
	type_(other.type_),
	meshName_(other.meshName_),
	skinName_(other.skinName_)
{
}

ModelID::~ModelID()
{
}

ModelID &ModelID::operator=(const ModelID &other)
{
	type_ = other.type_;
	meshName_ = other.meshName_;
	skinName_ = other.skinName_;
	return *this;
}

bool ModelID::initFromString(
		const char *type,
		const char *meshName,
		const char *skinName)
{
	type_ = type;
	meshName_ = meshName;
	skinName_ = skinName;
	return true;
}


bool ModelID::initFromNode(XMLNode *modelNode)
{
	XMLNode *typeNode = 0;
	if (!modelNode->getNamedParameter("type", typeNode)) return false;

	// Store the model type
	type_ = typeNode->getContent();

	// Create the correct model depending on the model type
	if (strcmp(typeNode->getContent(), "ase") == 0)
	{
		// 3DS Studio ASCII Files
		XMLNode *meshNode, *skinNode;
		if (!modelNode->getNamedChild("mesh", meshNode)) return false;
		const char *meshNameContent = meshNode->getContent();
		if (!S3D::fileExists(S3D::getModFile(meshNameContent)))
		{
			return modelNode->returnError(
				S3D::formatStringBuffer(
					"Mesg file \"%s\" does not exist",
					meshNameContent));
			return false;
		}

		if (!modelNode->getNamedChild("skin", skinNode)) return false;
		const char *skinNameContent = skinNode->getContent();
		if (strcmp(skinNameContent, "none") != 0)
		{
			if (!S3D::fileExists(S3D::getModFile(skinNameContent)))
			{
				return modelNode->returnError(
					S3D::formatStringBuffer(
						"Skin file \"%s\" does not exist",
						skinNameContent));
			}
		}

		meshName_ = meshNameContent;
		skinName_ = skinNameContent;
	}
	else if (strcmp(typeNode->getContent(), "MilkShape") == 0)
	{
		const char *meshNameContent = modelNode->getContent();
		if (!S3D::fileExists(S3D::getModFile(meshNameContent)))
		{
			return modelNode->returnError(
				S3D::formatStringBuffer(
					"Mesh file \"%s\" does not exist",
					meshNameContent));
		}

		meshName_ = meshNameContent;
	}
	else if (strcmp(typeNode->getContent(), "Tree") == 0)
	{
		std::string meshName;
		bool burnt, snow;
		if (!modelNode->getNamedChild("type", meshName)) return false;
		if (!modelNode->getNamedChild("snow", snow)) return false;
		if (!modelNode->getNamedChild("burnt", burnt)) return false;

		TreeModelFactory::TreeType normalType, burntType;
		if (!TreeModelFactory::getTypes(meshName.c_str(), true, 
			normalType, burntType))
		{
			return modelNode->returnError(
				S3D::formatStringBuffer(
					"Tree type \"%s\" does not exist",
					meshName.c_str()));
		}

		skinName_ = S3D::formatStringBuffer("%s", (snow?"S":"N"));
		meshName_ = S3D::formatStringBuffer("%s:%s", (burnt?"B":"N"), meshName.c_str());
	}
	else
	{
		return modelNode->returnError(
			S3D::formatStringBuffer(
				"Unknown mesh type \"%s\"",
				typeNode->getContent()));
	}

	return true;
}

bool ModelID::writeModelID(NetBuffer &buffer)
{
	buffer.addToBuffer(type_);
	buffer.addToBuffer(meshName_);
	buffer.addToBuffer(skinName_);
	return true;
}

bool ModelID::readModelID(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(type_)) return false;
	if (!reader.getFromBuffer(meshName_)) return false;
	if (!reader.getFromBuffer(skinName_)) return false;
	return true;	
}

const char *ModelID::getStringHash()
{ 
	hash_ = meshName_ + "-" + skinName_;
	return hash_.c_str(); 
}
