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
#include "coldet/coldetimpl.h"
#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include "paths.h"

#include "xml/tinyxml/tinyxml.h"

#include <malloc.h>


// The maximum size in characters that the name can be.
#define MAX_MODEL_NAME_LENGTH 24


#define MODEL_FORMAT_VERSION 4

struct modelVertex{
	/*	
	guVector v;			// Vertex
	guVector n;			// Normal
	//*/
    f32 v[3];				// (x,y,z) component
    f32 n[3];				// Vertex normal
	
};

struct modelFace{
    int v[3];				// 3 vertices used for this face
    f32 uv[6];				// uv texture coords. 
	u8 c[9];				// Color components for the 3 vertices
	u8 tIndex;				// Texture index for the face
};


class Model{   

public:

	Model();
	Model(const char* modelName, bool buildCollisionModel = false);
	~Model();
	//u32 getTextureId(int texture);
	//void setTextureId(u32 textureId, int texture);
	void deleteTexture(int texture);
	void render(Mtx modelview);
	int getNumberOfVertices(){return num_verts;};
	int getNumberOfFaces(){return num_faces;};
	int getNumberOfTextureIds(){return num_textures;};
	const char * getName(){return name;};
	//const char * getFileName(){return filename;};
	//char **getTextureNames(){return textureNames;};
	bool loadModel(const char* fileName, bool buildCollisionModel = false);
	
	
	CollisionModel3DImpl* collisionModel;
	
private:

    modelVertex*  vList;		// Vertices
    modelFace*    fList;		// Faces

	// Display List variables
	u8* displayList;
	u32 actualDLsize;			// Actual size of the display list
	
    f32 x,y,z;					// Location
    f32 ax,ay,az;				// Angles of rotation
    f32 sizeX, sizeY, sizeZ;	// Size
    int num_verts;				// Number of vertices in the model
    int num_faces;				// Number of faces in the model

	// Textures
    GXTexObj* textureObjs;
	// Not sure this is needed:
	//char **textureNames;
    u16 num_textures;

	//--DCN: Going to leave this out
	//char filename[256];
	char name[MAX_MODEL_NAME_LENGTH];
	
    
    void buildDisplayList();
    void loadTGATexture(const char *texture, GXTexObj& textureId);
    int untexturedFaces();

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
    Model* getModel(int index);
    Model* getModel(const char* name);

private:
    std::vector<Model*> list;
};

#endif
