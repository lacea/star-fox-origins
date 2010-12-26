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

#include "Model.h"

//----------------------------------------
//
// Default Constructor
//
// @pre     -
// @post    Creates an empty model
// @param   -
//----------------------------------------

Model::Model(): collisionModel(NULL),
				vList(NULL), fList(NULL), 
				displayList(NULL), actualDLsize(0), 
				num_verts(0), num_faces(0),
				textureObjs(NULL), 
				num_textures(0)
{
	memset(name,0,MAX_MODEL_NAME_LENGTH);
}

//----------------------------------------
//
// Constructor
//
// @pre     -
// @post    Creates large display list (perfect for temporaries)
// @param   fileName: The file to load in our model variables from.
// @param   buildCollisionModel: Build for collision detection?
//----------------------------------------

Model::Model(const char * fileName, bool buildCollisionModel): 
				collisionModel(NULL),
				vList(NULL), fList(NULL), 
				displayList(NULL), actualDLsize(0), 
				num_verts(0), num_faces(0),
				textureObjs(NULL), 
				num_textures(0)
{
	memset(name,0,MAX_MODEL_NAME_LENGTH);
	loadModel(fileName, buildCollisionModel);
}

//----------------------------------------
//
// Destructor
//
// @pre     -
// @post    Model has been eliminated
// @param   -
//----------------------------------------

Model::~Model(){

	delete[] vList; 
	vList = NULL;
	delete[] fList;
	fList = NULL;
	delete collisionModel;
	collisionModel = NULL;
	delete[] textureObjs;
	textureObjs = NULL;

	if(displayList)
		free(displayList);
	displayList = NULL;

}

//----------------------------------------
//
// Function: loadModel
//
//		Loads in the model from an xml file; builds a
//		display list for the model, and (if applicable)
//		builds a collision detection model.
//
// @pre     modelName MUST be a valid name!
// @post    The model has been loaded and is ready to go
// @param   modelName: The name of the model (used to load the file too)
// @param   buildCollisionModel: Build for collision detection?
// @return  bool: true if loaded, false if there was a problem
//
//----------------------------------------

bool Model::loadModel(const char* modelName,  bool buildCollisionModel){

	std::string fileName = PATH_MODELS;
	fileName += modelName;
	fileName += ".xml";

	TiXmlDocument *doc = new TiXmlDocument(fileName.c_str());
	if (!doc->LoadFile()){
		fprintf(stderr, "Can't open model: %s\n", fileName.c_str());
		return false;
	}
	
	TiXmlHandle docHandle(doc);
	TiXmlHandle model = docHandle.FirstChild("model");
	
	if(!model.Element()){
		fprintf(stderr, "No model information for: %s\n",  fileName.c_str());
		doc->Clear();
		delete doc;
		return false;
	}

	//TODO: We should make this dynamic:
	// Copy the name
	u32 nameLen = strlen(modelName);
	if(nameLen > MAX_MODEL_NAME_LENGTH)
		nameLen = MAX_MODEL_NAME_LENGTH;

	memcpy(name, modelName, nameLen);


	// Not needed
	//TiXmlElement* version = model->FirstChild("version").ToElement();
	//TiXmlElement* offset = model->FirstChild("offset").ToElement();
	TiXmlHandle node = model.FirstChild("textures");
	num_textures = atoi(node.Element()->Attribute("num"));

	if (num_textures > 0){
		textureObjs = new Texture[num_textures];
		std::string texturePath;

		for (u16 i = 0; i < num_textures; i++){

			TiXmlNode* texNode = model.FirstChild("textures").Child("name", i).Element();
			texturePath = PATH_TEXTURES;	
			texturePath += texNode->FirstChild()->Value();

			// Load the texture!
			loadInTexture(texturePath.c_str(), textureObjs[i]);  
		}
	}

	// Load the vertices

	TiXmlElement* verts = model.FirstChild("verts").Element();
	TiXmlNode* vert = verts->FirstChild("vert");

	num_verts = atoi(verts->Attribute("num"));
	vList = new modelVertex[num_verts];

	for (int i = 0; i < num_verts; i++){

		node = vert->FirstChild("pos");

		// Read in the vertex data
		vList[i].v.x = float(atof(node.Element()->Attribute("x")));
		vList[i].v.y = float(atof(node.Element()->Attribute("y")));
		vList[i].v.z = float(atof(node.Element()->Attribute("z")));

		node = vert->FirstChild("norm");

		// Read in the normal data
		vList[i].n.x = float(atof(node.Element()->Attribute("x")));
		vList[i].n.y = float(atof(node.Element()->Attribute("y")));
		vList[i].n.z = float(atof(node.Element()->Attribute("z")));	

		// Move to the next one
		vert = vert->NextSiblingElement("vert");
	}
	
	TiXmlElement* faces = model.FirstChild("faces").Element();
	TiXmlNode* face = faces->FirstChild("face");

	num_faces = atoi(faces->Attribute("num"));
	fList = new modelFace[num_faces];

	for (int i = 0; i < num_faces; i++){

		// Get the texture index
		fList[i].tIndex = atoi(face->ToElement()->Attribute("texIndex")) - 1;
			

		// We're recycling our pointers here:
		verts = face->FirstChild("verts")->ToElement();
		vert = verts->FirstChild("vert");

		// There are three vertices in all
		int j3 = 0;
		for(int j = 0; j < 3; j++){
			j3 = j*3; // To get the colors in the right spot

			// Get the index info
			fList[i].v[j] = atoi(vert->ToElement()->Attribute("index"));
			
			// Get the color info
			node = vert->FirstChild("color");

			fList[i].c[j3  ] = atoi(node.Element()->Attribute("r"));
			fList[i].c[j3+1] = atoi(node.Element()->Attribute("g"));
			fList[i].c[j3+2] = atoi(node.Element()->Attribute("b"));

			// Move to the next one
			vert = vert->NextSiblingElement("vert");

		}

		// All that's left is the uv data!

		// Make these into verts and vert:
		TiXmlElement* uvParent = face->FirstChild("uv")->ToElement();

		// Start with the u, then go to the v
		node = uvParent->FirstChild("u");

		fList[i].uv[0] = float(atof(node.Element()->Attribute("x")));
		fList[i].uv[1] = float(atof(node.Element()->Attribute("y")));
		fList[i].uv[2] = float(atof(node.Element()->Attribute("z")));	

		node = uvParent->FirstChild("v");

		fList[i].uv[3] = float(atof(node.Element()->Attribute("x")));
		fList[i].uv[4] = float(atof(node.Element()->Attribute("y")));
		fList[i].uv[5] = float(atof(node.Element()->Attribute("z")));			

		// Move to the next one
		face = face->NextSiblingElement("face");
	}
	

	// Build the display list with the variables gathered.
	buildDisplayList();

	// Make a new collision model based on our faces (there's probably a better way)
	
	//
	//
	//
	// Once again, I blame YOU! I don't think this is being built.
	//
	//
	//
	if(buildCollisionModel){
		collisionModel = new CollisionModel3DImpl(false);
	
		for (s32 i = 0; i < num_faces; ++i){
			//--DCN: A temporary measure, until the collision detection is redone.
			collisionModel->addTriangle(vList[fList[i].v[0]].v.x,
										vList[fList[i].v[0]].v.y,
										vList[fList[i].v[0]].v.z,
										vList[fList[i].v[1]].v.x,
										vList[fList[i].v[1]].v.y,
										vList[fList[i].v[1]].v.z,
										vList[fList[i].v[2]].v.x,
										vList[fList[i].v[2]].v.y,
										vList[fList[i].v[2]].v.z
										);
			//collisionModel->addTriangle(vList[fList[i].v[0]].v, vList[fList[i].v[1]].v, vList[fList[i].v[2]].v);
		
		}
		collisionModel->finalize();
	}

	// Clean up
	doc->Clear();
	delete doc;
		
	return true;
}

//----------------------------------------
//
// Function: untexturedFaces
//
//		Returns the number of faces without texturing in the model
//
// @pre     -
// @post    -
// @param   -
// @return  The number of untextured faces
//
//----------------------------------------

s32 Model::untexturedFaces(){
	if (num_faces == 0) return 0;
	int num = 0;    
	int i = num_faces - 1;
	do{
		if (fList[i].tIndex == -1)
			++num;
		--i;
	}while(i >= 0);

	return num;
}

//----------------------------------------
//
// Function: buildDisplayList
//
//		Builds the display list for the model
//
// @pre     -
// @post    The display list (displayList) has been populated
// @param   -
// @return  -
//
//----------------------------------------

void Model::buildDisplayList(){
	int j = 0;
	if (num_faces == 0) return;

	s32 unTexFaces = untexturedFaces();

	if(displayList)
		free(displayList);
	displayList = NULL;

	u8* tmpDisplayList = (u8*)(memalign(32, TEMP_DISPLIST_SIZE));

	if(!tmpDisplayList){
		// PANIC!
		exit(1);
	}

	DCInvalidateRange((void*)tmpDisplayList, TEMP_DISPLIST_SIZE);

	GX_BeginDispList(tmpDisplayList, TEMP_DISPLIST_SIZE);


	//--DCN: Might need to get rid of this one entirely,
	// or something else, because I'm not sure we're allowed
	// to do a "clearVtxDesc()" and such inside a display list.

	//*
	// Render the untextured faces
	if (unTexFaces > 0){

		//
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		//
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);


		GX_Begin(GX_TRIANGLES, GX_VTXFMT_CLR, 3*unTexFaces);
			for (s32 i = 0; i < unTexFaces; i++){

				GX_Position3f32(vList[fList[i].v[0]].v.x,
								vList[fList[i].v[0]].v.y,
								vList[fList[i].v[0]].v.z);
				GX_Color4u8(fList[i].c[0], fList[i].c[1], fList[i].c[2], 0xff);

				GX_Position3f32(vList[fList[i].v[1]].v.x,
								vList[fList[i].v[1]].v.y,
								vList[fList[i].v[1]].v.z);
				GX_Color4u8(fList[i].c[3], fList[i].c[4], fList[i].c[5], 0xff);

				GX_Position3f32(vList[fList[i].v[2]].v.x,
								vList[fList[i].v[2]].v.y,
								vList[fList[i].v[2]].v.z);
				GX_Color4u8(fList[i].c[6], fList[i].c[7], fList[i].c[8], 0xff);
			}

		GX_End();

	}
	// Render the textured faces
	if ((num_faces - unTexFaces) > 0){
		j = unTexFaces;

		// We are using colors, textures, and normals
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		//

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		//--DCN: This is a bad way to do things. BAD!
		// It's going by the textures, we can't use trianglefans.
		for (s32 i = 0; i< num_textures; ++i){

			//--DCN: Maybe have two arrays, one of data objects, one of GXTexObjs
			GX_LoadTexObj(&textureObjs[i].obj, GX_TEXMAP0);

			while(fList[j].tIndex == i && j < num_faces){

				// Fine, rather than being on the outside it'll be on the inside. 

				GX_Begin(GX_TRIANGLES, GX_VTXFMT_TEX_NRM_CLR, 3);

					GX_Position3f32(vList[fList[j].v[0]].v.x,
									vList[fList[j].v[0]].v.y,
									vList[fList[j].v[0]].v.z);
					GX_Normal3f32(vList[fList[j].v[0]].n.x,
								  vList[fList[j].v[0]].n.y,
								  vList[fList[j].v[0]].n.z);
					GX_TexCoord2f32(fList[j].uv[0], 1-fList[j].uv[1]);
					GX_Color4u8(fList[j].c[0],fList[j].c[1],fList[j].c[2], 0xff);
					//
					GX_Position3f32(vList[fList[j].v[1]].v.x,
									vList[fList[j].v[1]].v.y,
									vList[fList[j].v[1]].v.z);
					GX_Normal3f32(vList[fList[j].v[1]].n.x,
								  vList[fList[j].v[1]].n.y,
								  vList[fList[j].v[1]].n.z);
					GX_TexCoord2f32(fList[j].uv[2], 1-fList[j].uv[3]);
					GX_Color4u8(fList[j].c[3],fList[j].c[4],fList[j].c[5], 0xff);
					//
					GX_Position3f32(vList[fList[j].v[2]].v.x,
									vList[fList[j].v[2]].v.y,
									vList[fList[j].v[2]].v.z);
					GX_Normal3f32(vList[fList[j].v[2]].n.x,
								  vList[fList[j].v[2]].n.y,
								  vList[fList[j].v[2]].n.z);
					GX_TexCoord2f32(fList[j].uv[4], 1-fList[j].uv[5]);
					GX_Color4u8(fList[j].c[6],fList[j].c[7],fList[j].c[8], 0xff);

				GX_End();

				++j;

			}
		}
	}
	//*/

	// Copy the temporary DL into our permanent display list

	actualDLsize = GX_EndDispList();

	displayList = (u8*)(memalign(32, actualDLsize));

	DCInvalidateRange((void*)displayList, actualDLsize);

	memcpy(displayList, tmpDisplayList, actualDLsize);
	DCFlushRange((void*)displayList, actualDLsize);

	// Clean up
	if(tmpDisplayList)
		free(tmpDisplayList);
	tmpDisplayList = NULL;
}

//----------------------------------------
//
// Function: loadInTexture
//
//		Loads in a file for the texture.
//
// @pre     The texture to be loaded should exist!
// @post    The textures has been loaded and primed for use
// @param   fileName: The file name and path of the texture to load
// @param   textureObj: The texture object that we are populating
// @return  -
//
//----------------------------------------

void Model::loadInTexture(const char *fileName, Texture& texture){
	LoadTexture(fileName, texture);
}

//----------------------------------------
//
// Function: render
//
//		Render the model to screen
//
// @pre     -
// @post    The display list for the model has been called.
// @param   modelview: The model view matrix to be used for the model
// @return  -
//
//----------------------------------------

void Model::render(Mtx modelview){

	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	Mtx normMtx;
	//guMtxInverse(modelview, tempMtx);
    //guMtxTranspose(tempMtx, normMtx);
	guMtxInvXpose(modelview, normMtx);
	GX_LoadNrmMtxImm(normMtx, GX_PNMTX0);

	//--DCN: This is where the problem happens with asteroid:
	GX_CallDispList(displayList, actualDLsize);


}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//															//
//					 Star Fox Origins						//
//					     									//
//					  ModelList Class						//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

ModelList::ModelList(){}

ModelList::~ModelList(){
	clear();
}

void ModelList::push(Model* model){
	list.push_back(model);
}

void ModelList::clear(){
	while(!list.empty()){
		if(list.back())
    		delete list.back();
		list.pop_back();
	}
}

Model* ModelList::getModel(s32 index){
	if (u32(index) >= list.size())
		return NULL;
	return list.at(index);
}

Model* ModelList::getModel(const char* name){
	for(u32 i = 0, mListSize = list.size(); i < mListSize; ++i){
		if (strcmp(name, list[i]->getName()) == 0)
			return list[i];
	}
	return NULL;
}
