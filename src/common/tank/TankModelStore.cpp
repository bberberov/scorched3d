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

#include <tank/TankModelStore.h>
#include <3dsparse/ModelStore.h>
#include <3dsparse/Model.h>
#include <common/Defines.h>
#include <XML/XMLFile.h>
#include <lang/LangResource.h>

TankModelStore::TankModelStore()
{
	modelCatagories_.insert("All");
}

TankModelStore::~TankModelStore()
{
	clear();
}

bool TankModelStore::loadTankMeshes(ScorchedContext &context, 
	int detailLevel, ProgressCounter *counter)
{
	clear();

	// Load tank definition file
	if (counter) counter->setNewOp(LANG_RESOURCE("LOADING_TANKS", "Loading tanks"));
	XMLFile file;
	if (!file.readFile(S3D::getModFile("data/tanks.xml")))
	{
		S3D::dialogMessage("Scorched Models", S3D::formatStringBuffer(
					  "Failed to parse tanks.xml\n%s", 
					  file.getParserError()));
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		S3D::dialogMessage("Scorched Models",
					"Failed to find tank definition file \"data/tanks.xml\"");
		return false;		
	}

	// Itterate all of the tanks in the file
	int count = 0;
	std::vector<TankModel *> randomModels, lowModels, midModels, highModels;
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        ++childrenItor, count++)
    {
		if (counter) counter->
			setNewPercentage(float(count) / float(children.size()) * 100.0f);
		// Parse the tank entry
        XMLNode *currentNode = (*childrenItor);
		if (stricmp(currentNode->getName(), "tank"))
		{
			return currentNode->returnError("Failed to tank node");
		}

		TankModel *tankModel = new TankModel();
		if (!tankModel->initFromXML(context, currentNode))
		{
			return currentNode->returnError("Failed to parse tank node");;
		}

		// Add models depending on the number of triangles in each model
		Model *model = ModelStore::instance()->loadModel(
			tankModel->getTankModelID());
		if (strcmp(tankModel->getName(), "Random") == 0)
		{
			randomModels.push_back(tankModel);
		}
		else 
		{
			int triangles = model->getNumberTriangles();
			if (triangles <= 250) lowModels.push_back(tankModel);
			else if (triangles <= 500) midModels.push_back(tankModel);
			else highModels.push_back(tankModel);
		}
	}

	// Add tanks dependant on the set detail level
	if (detailLevel >= 2) addModels(highModels);
	if (detailLevel >= 1) addModels(midModels);
	if (detailLevel >= 0) addModels(lowModels);
	if (models_.empty()) addModels(midModels);
	if (models_.empty()) addModels(highModels);
	addModels(randomModels);

	// Remove any models we don't use
	killModels(randomModels);
	killModels(lowModels);
	killModels(midModels);
	killModels(highModels);

	return true;
}

void TankModelStore::addModels(std::vector<TankModel *> &src)
{
	while (!src.empty())
	{
		TankModel *tankModel = src.back();
		src.pop_back();

		// Add catagories to list of all catagories
		std::set<std::string> &catagories = tankModel->getCatagories();
		std::set<std::string>::iterator catitor;
		for (catitor = catagories.begin();
			catitor != catagories.end();
			++catitor)
		{
			modelCatagories_.insert((*catitor).c_str());
		}

		models_.push_back(tankModel);
	}
}

void TankModelStore::killModels(std::vector<TankModel *> &src)
{
	while (!src.empty())
	{
		TankModel *tankModel = src.back();
		src.pop_back();
		delete tankModel;
	}
}

TankModel *TankModelStore::getRandomModel(int team, bool ai, const char *tankType)
{
	std::vector<TankModel *> models;
	std::vector<TankModel *>::iterator itor;
	for (itor = models_.begin();
		itor != models_.end();
		++itor)
	{
		TankModel *model = (*itor);
		if (strcmp(model->getName(), "Random") != 0)
		{
			if (model->isOfTeam(team) && 
				model->isOfAi(ai) &&
				model->isOfTankType(tankType)) 
			{
					models.push_back(model);
			}
		}
	}

	if (models.empty()) 
	{
		S3D::dialogExit("TankModelStore",
			S3D::formatStringBuffer("Failed to find any tank models that match the "
				"current team (%i), ai (%s) and tanktype (%s)",
				team, ai?"yes":"no", tankType));
	}
	unsigned int randomNumber = rand();
	TankModel *model = models[randomNumber % models.size()];
	return model;
}

TankModel *TankModelStore::getModelByName(const char *name)
{
	DIALOG_ASSERT(models_.size());

	if (strcmp(name, "Random") == 0) return 0;

	std::vector<TankModel *>::iterator itor;
	for (itor = models_.begin();
		 itor != models_.end();
		 ++itor)
	{
		TankModel *current = (*itor);
		if (0 == strcmp(current->getName(), name))
		{
			return current;
		}
	}

	return 0;
}

void TankModelStore::clear() 
{
	std::vector<TankModel *>::iterator itor;
	for (itor = models_.begin();
		itor != models_.end();
		++itor)
	{
		delete *itor;
	}
	models_.clear();
	modelCatagories_.clear();
}
