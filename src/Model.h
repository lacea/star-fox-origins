/*  Copyright (C) 2010 Michael Wells (dancinninjac@gmail.com)

    This file is part of StarFox Origins.

    StarFox Origins is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    StarFox Origins is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with StarFox Origins.  If not, see <http://www.gnu.org/licenses/>.
*/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//															//
//					 Star Fox Origins						//
//					     									//
//					    Model Class							//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// The 3D models that are displayed on the screen

#ifndef __MODEL_H__
#define __MODEL_H__

#include "StdInc.h"

#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include <malloc.h>
#include "paths.h"
#include "TextureMgr.h"
#include "xml/tinyxml/tinyxml.h"
#include "coldet/coldetimpl.h"

// The maximum size in characters that the name can be.
#define MAX_MODEL_NAME_LENGTH 24

#define MODEL_FORMAT_VERSION 4


struct modelVertex{
	guVector v;			// Vertex (x,y,z) component
	guVector n;			// Vertex normal
};

struct modelFace{
    s32 v[3];				// 3 vertices used for this face
    f32 uv[6];				// uv texture coords. 
	u8  c[9];				// Color components for the 3 vertices
	s8  tIndex;				// Texture index for the face
};


class Model{   

public:

	Model();
	Model(const char* modelName, bool buildCollisionModel = false);
	~Model();

	void render(Mtx modelview);
	//void deleteTexture(s32 texture);
	//s32 getNumberOfVertices(){return num_verts;};
	//s32 getNumberOfFaces(){return num_faces;};
	//s32 getNumberOfTextureIds(){return num_textures;};
	const char* getName(){return name;};

	bool loadModel(const char* fileName, bool buildCollisionModel = false);
	
	
	CollisionModel3DImpl* collisionModel;
	
private:
	
	// Temporary display list size (really big to fit everything in)
	static const u32 TEMP_DISPLIST_SIZE = 32768;

    modelVertex*  vList;		// Vertices
    modelFace*    fList;		// Faces

	// Display List variables
	u8* displayList;
	u32 actualDLsize;			// Actual size of the display list
	
    s32 num_verts;				// Number of vertices in the model
    s32 num_faces;				// Number of faces in the model

	// Textures
	//TODO: Make everything load into the texture manager, maybe?
    Texture* textureObjs;
    u16 num_textures;

	char name[MAX_MODEL_NAME_LENGTH];
	
    
    void buildDisplayList();
    void loadInTexture(const char* fileName, Texture& texture);
    s32 untexturedFaces();

};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//															//
//					 Star Fox Origins						//
//					     									//
//					  ModelList Class						//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// For storing the list of Models

class ModelList{

public:
    ModelList();
   ~ModelList();
    void push(Model* model);
    void clear();
    Model* getModel(s32 index);
    Model* getModel(const char* name);

private:
    std::vector<Model*> list;
};

#endif
