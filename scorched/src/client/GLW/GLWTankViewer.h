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

#ifndef _gLWTankViewer_h
#define _gLWTankViewer_h

#include <GLW/GLWScrollW.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWDropDownText.h>
#include <GLW/GLWToolTip.h>
#include <tank/TankModel.h>
#include <tankgraph/ModelRendererTank.h>
#include <vector>

class GLWTankViewer : public GLWidget,
					  public GLWDropDownI
{
public:
	GLWTankViewer(float x = 0.0f, float y = 0.0f, 
		int numH = 0, int numV = 0);
	virtual ~GLWTankViewer();

	void init();

	void selectModelByName(const char *name);
	const char *getModelName();
	void setTeam(int team);
	void setTankType(const std::string &tankType);

	// Inhertied from GLWDropDownI
	virtual void select(unsigned int id, const int pos, GLWSelectorEntry value);

	// Inhertied from GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, 
						   bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	REGISTER_CLASS_HEADER(GLWTankViewer);
protected:
	struct ModelEntry
	{
		TankModel *model;
		ModelRendererTank *mesh;
	};

	GLWDropDownText catagoryChoice_;
	GLWScrollW scrollBar_;
	GLWPanel infoWindow_;
	ToolTip toolTip_;
	std::vector<ModelEntry> models_;
	int numH_, numV_;
	float rot_;
	float rotXY_, rotYZ_;
	float rotXYD_, rotYZD_;
	float totalTime_;
	int selected_;
	int team_;
	std::string tankType_;

	void refreshAvailableModels();
	void setTankModels(std::vector<ModelEntry> &models);
	virtual void drawItem(int pos, bool selected);
	virtual void drawCaption(int pos);

private:
	GLWTankViewer(const GLWTankViewer &);
	const GLWTankViewer & operator=(const GLWTankViewer &);

};

#endif // _gLWTankViewer_h

