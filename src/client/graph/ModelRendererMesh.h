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

#if !defined(__INCLUDE_ModelRendererMeshh_INCLUDE__)
#define __INCLUDE_ModelRendererMeshh_INCLUDE__

#include <graph/ModelRenderer.h>
#include <3dsparse/Model.h>

class GLTexture;
class ModelRendererMesh : public ModelRenderer
{
public:
	ModelRendererMesh(Model *model);
	virtual ~ModelRendererMesh();

	virtual void draw(float currentFrame, 
		float distance, float fade, bool setState);
	virtual void drawBottomAligned(float currentFrame, 
		float distance, float fade, bool setState);

	virtual Model *getModel() { return model_; }

	virtual Mesh *drawModel(float currentFrame, 
		float distance, float fade, bool setState, 
		std::vector<Mesh *> &meshes, Mesh *lastMesh);

	void setVertexTranslation(FixedVector &translation) { vertexTranslation_ = translation; }

protected:
	struct MeshFrameInfo
	{
		MeshFrameInfo() : displayList(0) {}

		unsigned int displayList;
	};

	Model *model_;
	std::vector<BoneType *> boneTypes_;
	std::vector<MeshFrameInfo> frameInfos_;
	FixedVector vertexTranslation_;

	struct TriangleInfo
	{
		Vector position;
		Vector normal;
		Vector color;
		float texCoordx, texCoordy;
	};

	virtual void drawMesh(Mesh *mesh, Mesh *lastMesh,
		int frame, bool useTextures, bool vertexLighting);
	virtual void drawVerts(Mesh *mesh, bool vertexLighting, int frame);
	virtual void setup();
};

#endif // __INCLUDE_ModelRendererMeshh_INCLUDE__
