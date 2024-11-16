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

#include <GLW/GLWTankViewer.h>
#include <tank/TankModelStore.h>
#include <tankgraph/ModelRendererTankStore.h>
#include <3dsparse/ModelStore.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/DefinesString.h>
#include <lang/LangResource.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>

static const float TankSquareSize = 90.0f;
static const float TankHalfSquareSize = TankSquareSize / 2.0f;
static const float TankPadding = 20.0f;
static const float TankInfo = 320.0f;

GLWTankViewer::GLWTankViewer(float x, float y, int numH, int numV) :
	GLWidget(x, y, 
					 TankSquareSize * numH + TankPadding, 
					 TankSquareSize * numV + TankPadding),
	scrollBar_(w_ - 12.0f, y + 2.0f, h_ - 4.0f, 0, 0, numV),
	infoWindow_(x + TankSquareSize * numH + TankPadding + 10.0f, 
				y + TankSquareSize * numV + TankPadding - TankInfo + 30.0f, 
				TankInfo, TankInfo, true),
	numH_(numH), numV_(numV),
	rot_(0.0f), selected_(0),
	rotXY_(0.0f), rotYZ_(0.0f), 
	rotXYD_(1.0f), rotYZD_(1.0f),
	totalTime_(0.0f),
	team_(0),
	catagoryChoice_(x, 
					y + TankSquareSize * numV + TankPadding + 5.0f,
					+ TankSquareSize * numH + TankPadding)
{
	catagoryChoice_.setHandler(this);

	catagoryChoice_.setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("MODEL_CATAGORY", "Model Catagory"),
		LANG_RESOURCE("MODEL_CATAGORY_TOOLTIP", 
		"Displays the currently selected model catagory.\n"
		"To make models easier to locate\n"
		"tank models are grouped by catagory.")));
	infoWindow_.setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CURRENT_MODEL", "Current Model"),
		LANG_RESOURCE("CURRENT_MODEL_TOOLTIP", 
		"Displays the currently selected tank model.\n"
		"This is the model this player will use in game.\n"
		"Choose a new model from the selection on the\n"
		"left.")));
}

GLWTankViewer::~GLWTankViewer()
{

}

void GLWTankViewer::init()
{
	models_.clear();
	std::set<std::string> &catagories = 
		ScorchedClient::instance()->getTankModels().getModelCatagories();
	std::set<std::string>::iterator catItor;
	for (catItor = catagories.begin();
		 catItor != catagories.end();
		 ++catItor)
	{
		catagoryChoice_.addText(LANG_RESOURCE(*catItor, *catItor), (*catItor));
	}
	catagoryChoice_.setCurrentPosition(0);
	refreshAvailableModels();
}

void GLWTankViewer::setTeam(int team)
{
	team_ = team;
	refreshAvailableModels();
}

void GLWTankViewer::setTankType(const std::string &tankType)
{
	tankType_ = tankType;
	refreshAvailableModels();
}

void GLWTankViewer::select(unsigned int id, 
						   const int pos, 
						   GLWSelectorEntry value)
{
	refreshAvailableModels();
}

void GLWTankViewer::refreshAvailableModels()
{
	GLWSelectorEntry *catagoryEntry = catagoryChoice_.getCurrentEntry();
	if (!catagoryEntry) return;

	std::vector<ModelEntry> newmodels;
	std::vector<TankModel *> &models = 
		ScorchedClient::instance()->getTankModels().getModels();
	std::vector<TankModel *>::iterator modelItor;
	for (modelItor = models.begin();
		 modelItor != models.end();
		 ++modelItor)
	{
		TankModel *tankModel = (*modelItor);
		if (tankModel->isOfCatagory(catagoryEntry->getDataText()))
		{
			// Check if this tank is allowed for this team
			if (!tankModel->isOfTeam(team_) ||
				!tankModel->isOfAi(false) ||
				!tankModel->isOfTankType(tankType_.c_str()))
			{
				continue;
			}

			ModelEntry entry;
			entry.model = tankModel;
			entry.mesh = 0;
			newmodels.push_back(entry);
		}
	}

	// Save current model
	int scrollCurrent = scrollBar_.getCurrent();
	TankModel *model = 0;
	if (selected_ >= 0 &&
		selected_ < (int) models_.size())
	{
		model = models_[selected_].model;
	}

	setTankModels(newmodels);

	// Select old model
	scrollBar_.setCurrent(scrollCurrent);
	for (int i=0; i<(int)models_.size(); i++)
	{
		if (models_[i].model == model) selected_ = i;
	}
}

void GLWTankViewer::setTankModels(std::vector<ModelEntry> &models)
{
	models_.clear();
	models_ = models;
	scrollBar_.setMax((int) (models_.size() / numH_) + 1);
	scrollBar_.setCurrent(0);
	selected_ = 0;

	// Sort the models 
	bool sorted = false;
	while (!sorted)
	{
		sorted = true;
		for (int i=0; i<(int) models_.size()-1; i++)
		{
			ModelEntry tmp = models_[i];
			ModelEntry test = models_[i+1];
			if (test.model->lessThan(tmp.model))
			{
				models_[i] = test;
				models_[i+1] = tmp;
				sorted = false;
			}
		}
	}

	// Move random model to the top
	std::vector<ModelEntry>::iterator itor;
	for (itor = models_.begin();
		itor != models_.end();
		++itor)
	{
		ModelEntry entry = (*itor);
		if (0==strcmp(entry.model->getName(),"Random"))
		{
			models_.erase(itor);
			std::vector<ModelEntry> tmpVector;
			tmpVector.push_back(entry);
			tmpVector.insert(tmpVector.end(), models_.begin(), models_.end());
			models_ = tmpVector;
			break;
		}
	}
}

const char *GLWTankViewer::getModelName()
{
	const char *name = "None";
	if (selected_ >= 0 &&
		selected_ < (int) models_.size())
	{
		name = models_[selected_].model->getName();
	}
	return name;
}

void GLWTankViewer::simulate(float frameTime)
{
	totalTime_ += frameTime;
	rot_ += frameTime * 45.0f;
	rotXY_ += frameTime * rotXYD_ * 5.0f;
	rotYZ_ += frameTime * rotYZD_ * 5.0f;
	if (rotXY_ < -45.0f)
	{
		rotXY_ = -45.0f;
		rotXYD_ = 1.0f;
	}
	else if (rotXY_ > 45.0f)
	{
		rotXY_ = 45.0f;
		rotXYD_ = -1.0f;
	}
	if (rotYZ_ < 0.0f)
	{
		rotYZ_ = 0.0f;
		rotYZD_ = 1.0f;
	}
	else if (rotYZ_ > 45.0f)
	{
		rotYZ_ = 45.0f;
		rotYZD_ = -1.0f;
	}

	scrollBar_.simulate(frameTime);
}

void GLWTankViewer::draw()
{
	Vector4 sunPosition(-100.0f, 100.0f, 400.0f, 1.0f);
	Vector4 sunDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
	Vector4 sunAmbient(0.4f, 0.4f, 0.4f, 1.0f);
	glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);

	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 6.0f, false);
	glEnd();

	infoWindow_.draw();
	scrollBar_.draw();

	GLState depthState(GLState::DEPTH_ON);
	const float heightdiv = (h_ / float(numV_));
	const float widthdiv = ((w_ - TankPadding - 10) / float(numH_));

	int pos = 0;
	for (int posV=0; posV<numV_; posV++)
	{
		for (int posH=0; posH<numH_; posH++, pos++)
		{
			int vectorPos = (scrollBar_.getCurrent() * numH_) + pos;
			if (vectorPos < (int) models_.size() &&
				vectorPos >= 0)
			{
				float posX = x_ + widthdiv * posH + TankHalfSquareSize + 2.0f;
				float posY = y_ + heightdiv * posV + TankHalfSquareSize + 2.0f;

				bool currselected = (vectorPos == selected_);
				if (currselected)
				{
					glColor3f(0.4f, 0.4f, 0.6f);
					glBegin(GL_LINE_LOOP);
						float SelectSize = TankHalfSquareSize - 2.0f;
						glVertex2f(posX - SelectSize, posY - SelectSize);
						glVertex2f(posX + SelectSize, posY - SelectSize);
						glVertex2f(posX + SelectSize, posY + SelectSize);
						glVertex2f(posX - SelectSize, posY + SelectSize);
					glEnd();
				}

				if (GLWToolTip::instance()->addToolTip(&toolTip_, 
					GLWTranslate::getPosX() + posX - TankHalfSquareSize, 
					GLWTranslate::getPosY() + posY - TankHalfSquareSize, 
					TankSquareSize,
					TankSquareSize))
				{
					toolTip_.setText(
						ToolTip::ToolTipInfo, 
						LANG_STRING(models_[vectorPos].model->getName()),
						LANG_STRING(models_[vectorPos].model->getTankModelID().getType()));	
				}

				float scale = 22.0f / 60.0f * TankSquareSize;
				glPushMatrix();
					glTranslatef(posX, posY - 5.0f, 0.0f);
					glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

					glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
					if (currselected) glRotatef(rot_, 0.0f, 0.0f, 1.0f);
					glScalef(scale, scale, scale);

					drawItem(vectorPos, currselected);
				glPopMatrix();
			}
		}
	}

	if (selected_ >= 0 &&
		selected_ < (int) models_.size())
	{
		const float infoX = infoWindow_.getX() + (infoWindow_.getW() / 2.0f);
		const float infoY = infoWindow_.getY() + (infoWindow_.getH() / 2.0f) - 35.0f;
		glPushMatrix();
			glTranslatef(infoX, infoY, 0.0f);
			glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

			drawCaption(selected_);
			glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(rot_, 0.0f, 0.0f, 1.0f);
			glScalef(100.0f, 100.0f, 100.0f);
	
			drawItem(selected_, true);
		glPopMatrix();
	}

	GLState depthStateOff(GLState::DEPTH_OFF);
	catagoryChoice_.draw();
}

void GLWTankViewer::mouseDown(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDown(button, x, y, skipRest);
	if (!skipRest)
	{
		catagoryChoice_.mouseDown(button, x, y, skipRest);
		if (!skipRest)
		{
			if (inBox(x, y, x_, y_, w_, h_))
			{
				int posY = int((y - y_) / (h_ / numV_));
				int posX = int((x - x_) / (w_ / numH_));
				
				int vectorPos = posX + posY * numH_ + scrollBar_.getCurrent() * numH_;
				if (vectorPos < (int) models_.size() &&
					vectorPos >= 0)
				{
					selected_ = vectorPos;
				}
			}
		}
	}
}

void GLWTankViewer::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	scrollBar_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWTankViewer::mouseUp(int button, float x, float y, bool &skipRest)
{
	scrollBar_.mouseUp(button, x, y, skipRest);
}

void GLWTankViewer::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (z < 0.0f) scrollBar_.setCurrent(scrollBar_.getCurrent() + 1);
		else scrollBar_.setCurrent(scrollBar_.getCurrent() - 1);
	}
}

void GLWTankViewer::drawCaption(int pos)
{
	GLState state(GLState::DEPTH_OFF);

	LANG_RESOURCE_VAR_1(TANK_NAME, "TANK_NAME", "Tank Name : {0}", models_[pos].model->getName());

	Vector color(0.3f, 0.3f, 0.3f);
	GLWFont::instance()->getGameFont()->
		drawWidth(TankInfo - 20.0f, 
			color, 10.0f, -150.0f, 175.0f, 0.0f, 
			TANK_NAME);
}
void GLWTankViewer::drawItem(int pos, bool selected)
{
	ModelRendererTank *mesh = models_[pos].mesh;
	if (!mesh)
	{
		mesh = ModelRendererTankStore::instance()->getMesh(
			models_[pos].model->getTankModelID());
		models_[pos].mesh = mesh;
	}

	// Tank
	Vector4 tankRot(1.0f, 0.0f, 0.0f, 0.0f);
	float matrix[16];
	tankRot.getOpenGLRotationMatrix(matrix);

	Vector tankPos;
	if (selected)
	{
		mesh->draw(totalTime_ * 20.0f, 
			matrix, tankPos, 0.0f, rotXY_, rotYZ_);
	}
	else
	{
		mesh->draw(totalTime_ * 20.0f, 
			matrix, tankPos, 0.0f, 45.0f, 45.0f);
	}

	// Ground
	GLState state(GLState::TEXTURE_OFF);
	glColor3f(181.0f / 255.0f, 204.0f / 255.0f, 237.0f / 255.0f);
	glBegin(GL_QUADS);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, -1.0f);
	glEnd();

	//Shadow
	/*{
		glDepthMask(GL_FALSE);
		glPushMatrix();
			glScalef(1.0f, 1.0f, 0.0f);
			models[pos]->draw(false, tankPos, 0.0f, rotXY_, rotYZ_);
		glPopMatrix();
		glDepthMask(GL_TRUE);
	}*/
}

void GLWTankViewer::selectModelByName(const char *name)
{
	DIALOG_ASSERT(models_.size());

	// Select the appropriate model
	int currentSel = 0;
	std::vector<ModelEntry>::iterator itor;
	for (itor = models_.begin();
		 itor != models_.end();
		 ++itor, currentSel ++)
	{
		TankModel *current = (*itor).model;
		if (0 == strcmp(current->getName(), name))
		{
			selected_ = currentSel;
			int steps = selected_ / numH_;
			scrollBar_.setCurrent(steps);
			return;
		}
	}
}
