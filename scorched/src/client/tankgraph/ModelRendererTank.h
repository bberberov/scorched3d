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

#if !defined(AFX_ModelRendererTank_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_)
#define AFX_ModelRendererTank_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_

#include <graph/ModelRendererMesh.h>

class ModelRendererTank : public ModelRenderer
{
public:
	ModelRendererTank(Model *tank);
	virtual ~ModelRendererTank();

	virtual void draw(float currentFrame, 
		float distance, float fade, bool setState);
	virtual void drawBottomAligned(float currentFrame, 
		float distance, float fade, bool setState);

	virtual Model *getModel() { return model_; }

	void draw(float frame, float *rotMatrix, Vector &position, 
		float fireOffSet, float rotXY, float rotXZ,
		bool absCenter = false, float scale = 1.0f, float fade = 1.0f, 
		bool setState = true);
	int getNoTris();

	FixedVector &getGunOffSet() { return gunOffset_; }
	FixedVector &getTurretCenter() { return turretCenter_; }
	float getScale() { return scale_; }
protected:
	Model *model_;
	float fireOffSet_;
	float scale_;
	float rotXY_;
	float rotXZ_;
	FixedVector gunOffset_;
	FixedVector turretCenter_;

	void setupModelRendererTank();

	std::vector<Mesh *> normalMeshes_;
	std::vector<Mesh *> turretMeshes_, gunMeshes_;
	ModelRendererMesh normalRenderer_;
	ModelRendererMesh turretRenderer_, gunRenderer_;
};

#endif // !defined(AFX_ModelRendererTank_H__CB857C65_A22F_4FBC_9344_EFF22F8A4EEA__INCLUDED_)
