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

#include <tankgraph/ModelRendererTankStore.h>
#include <3dsparse/ModelStore.h>

ModelRendererTankStore *ModelRendererTankStore::instance_ = 0;

ModelRendererTankStore *ModelRendererTankStore::instance()
{
	if (!instance_) instance_ = new ModelRendererTankStore();
	return instance_;
}

ModelRendererTankStore::ModelRendererTankStore()
{
}

ModelRendererTankStore::~ModelRendererTankStore()
{
}

ModelRendererTank *ModelRendererTankStore::getMesh(ModelID modelId)
{
	std::map<std::string, ModelRendererTank*>::iterator findItor = 
		meshes_.find(modelId.getStringHash());

	ModelRendererTank *mesh = 0;
	if (findItor == meshes_.end())
	{
		Model *newFile = ModelStore::instance()->loadModel(modelId);
		if (!newFile) return 0;

		// Create tank mesh
		mesh = new ModelRendererTank(newFile);
		meshes_[modelId.getStringHash()] = mesh;
	}
	else
	{
		mesh = (*findItor).second;
	}
	
	return mesh;
}
