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
#include <3dsparse/MSModelFactory.h>
#include <3dsparse/TreeModelFactory.h>
#include <3dsparse/ModelStore.h>
#include <3dsparse/Model.h>
#include <common/Defines.h>

ModelStore *ModelStore::instance_ = 0;

ModelStore *ModelStore::instance()
{
	if (!instance_)
	{
		instance_ = new ModelStore;
	}
	return instance_;
}

ModelStore::ModelStore()
{
}

ModelStore::~ModelStore()
{
}

Model *ModelStore::loadModel(ModelID &modelId)
{
	std::map<std::string, Model *>::iterator findItor =
		fileMap_.find(modelId.getStringHash());
	if (findItor == fileMap_.end())
	{
		Model *model = getModel(modelId);
		fileMap_[modelId.getStringHash()] = model;
		return model;
	}
	return (*findItor).second;
}

Model *ModelStore::getModel(ModelID &id)
{
	Model *model = 0;
	if (0 == strcmp(id.getType(), "ase"))
	{
		// Load the ASEFile containing the tank definitions
		std::string meshName(S3D::getModFile(id.getMeshName()));

		bool noSkin = 
			(0 == strcmp("none", id.getSkinName()));
		ASEModelFactory factory;
		std::string skinName = S3D::getModFile(id.getSkinName());
		model = factory.createModel(meshName.c_str(), 
			(noSkin?"":skinName.c_str()));
	}
	else if (0 == strcmp(id.getType(), "MilkShape"))
	{
		// Load the Milkshape containing the tank definitions
		std::string meshName(S3D::getModFile(id.getMeshName()));
		MSModelFactory factory;
		model = factory.createModel(meshName.c_str());		
	}
	else if (0 == strcmp(id.getType(), "Tree"))
	{
		// Create a model for the tree
		TreeModelFactory factory;
		model = factory.createModel(id.getMeshName());		
	}
	else
	{
		DIALOG_ASSERT(0);
	}

	return model;
}
