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

#include <graph/ModelRendererStore.h>
#include <graph/ModelRendererMesh.h>
#include <graph/ModelRendererTree.h>
#include <3dsparse/ModelStore.h>
#include <common/Defines.h>

ModelRendererStore *ModelRendererStore::instance_ = 0;

ModelRendererStore *ModelRendererStore::instance()
{
	if (!instance_)
	{
		instance_ = new ModelRendererStore;
	}
	return instance_;
}

ModelRendererStore::ModelRendererStore()
{
}

ModelRendererStore::~ModelRendererStore()
{
}

ModelRenderer *ModelRendererStore::loadModel(ModelID &modelId)
{
	std::map<std::string, ModelRenderer *>::iterator findItor =
		fileMap_.find(modelId.getStringHash());
	if (findItor == fileMap_.end())
	{
		ModelRenderer *model = getModel(modelId);
		fileMap_[modelId.getStringHash()] = model;
		return model;
	}
	return (*findItor).second;
}

ModelRenderer *ModelRendererStore::getModel(ModelID &id)
{
	Model *model = ModelStore::instance()->loadModel(id);
	ModelRenderer *modelRenderer = 0;
	if (0 == strcmp("Tree", id.getType()))
	{
		modelRenderer = new ModelRendererTree(model, id);
	}
	else
	{
		modelRenderer = new ModelRendererMesh(model);
	}
	return modelRenderer;
}
