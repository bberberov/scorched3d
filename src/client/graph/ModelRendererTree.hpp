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

#if !defined(__INCLUDE_ModelRendererTreeh_INCLUDE__)
#define __INCLUDE_ModelRendererTreeh_INCLUDE__

#include <graph/ModelRenderer.h>
#include <GLEXT/GLTextureReference.h>
#include <3dsparse/Model.h>
#include <common/ModelID.h>

class ModelRendererTree : public ModelRenderer
{
public:
	ModelRendererTree(Model *model, ModelID &id);
	virtual ~ModelRendererTree();

	virtual void draw(float currentFrame, 
		float distance, float fade, bool setState);
	virtual void drawBottomAligned(float currentFrame, 
		float distance, float fade, bool setState);

	virtual Model *getModel() { return model_; }

	static void setSkipPre(bool skip) { skipPre_ = skip; }
	static void drawInternalPre(bool setState);
	void drawInternal(float distance, float fade, bool setState);

protected:
	Model *model_;
	int treeType_;

	static bool skipPre_;
	static GLuint treePineList, treePineSmallList;
	static GLuint treePine2List, treePine2SmallList;
	static GLuint treePine3List, treePine3SmallList;
	static GLuint treePine4List, treePine4SmallList;
	static GLuint treePine2SnowList, treePine2SnowSmallList;
	static GLuint treePine3SnowList, treePine3SnowSmallList;
	static GLuint treePine4SnowList, treePine4SnowSmallList;
	static GLuint treePineSnowList, treePineSnowSmallList;
	static GLuint treePineBurntList, treePineBurntSmallList;
	static GLuint treePineYellowList, treePineYellowSmallList;
	static GLuint treePineLightList, treePineLightSmallList;
	static GLuint treePalmList, treePalmSmallList;
	static GLuint treePalm2List, treePalm2SmallList;
	static GLuint treePalm3List, treePalm3SmallList;
	static GLuint treePalm4List, treePalm4SmallList;
	static GLuint treePalmBList;
	static GLuint treePalmB2List;
	static GLuint treePalmB3List;
	static GLuint treePalmB4List;
	static GLuint treePalmB5List;
	static GLuint treePalmB6List;
	static GLuint treePalmB7List;
	static GLuint treePalmBurntList, treePalmBurntSmallList;
	static GLuint treeOakList, treeOakSmallList;
	static GLuint treeOak2List, treeOak2SmallList;
	static GLuint treeOak3List, treeOak3SmallList;
	static GLuint treeOak4List, treeOak4SmallList;

	static GLTextureReference pineTextureA_;
	static GLTextureReference pineTextureB_;
	static GLTextureReference palmTextureA_;
	static GLTextureReference palmTextureB_;
	static GLTextureReference oakTextureA_;
};

#endif // __INCLUDE_ModelRendererTreeh_INCLUDE__
