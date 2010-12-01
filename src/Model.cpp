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
				x(0.0f), y(0.0f), z(0.0f),
				ax(0.0f), ay(0.0f), az(0.0f),
				sizeX(0.0f), sizeY(0.0f), sizeZ(0.0f),
				num_verts(0), num_faces(0),
				textureObjs(NULL), 
				//textureNames(NULL), 
				num_textures(0)
{
	
	memset(name,0,MAX_MODEL_NAME_LENGTH);
	//memset(filename,0,256);

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
				x(0.0f), y(0.0f), z(0.0f),
				ax(0.0f), ay(0.0f), az(0.0f),
				sizeX(0.0f), sizeY(0.0f), sizeZ(0.0f),
				num_verts(0), num_faces(0),
				textureObjs(NULL), 
				//textureNames(NULL), 
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
	memcpy(name, modelName, MAX_MODEL_NAME_LENGTH);


	// Not needed
	//TiXmlElement* version = model->FirstChild("version").ToElement();
	//TiXmlElement* offset = model->FirstChild("offset").ToElement();
	TiXmlHandle node = model.FirstChild("textures");
	num_textures = atoi(node.Element()->Attribute("num"));

	if (num_textures > 0){
		textureObjs = new GXTexObj[num_textures];
		std::string texture;

		for (u16 i = 0; i < num_textures; i++){

			TiXmlNode* texNode = model.FirstChild("textures").Child("name", i).Element();
			texture = PATH_TEXTURES;	
			texture += texNode->FirstChild()->Value();

			// Load the texture!
			loadTGATexture(texture.c_str(), textureObjs[i]);  
		}
	}

	// Textures loaded, moving on to vertices.


	TiXmlElement* verts = model.FirstChild("verts").Element();
	TiXmlNode* vert = verts->FirstChild("vert");

	num_verts = atoi(verts->Attribute("num"));
	vList = new modelVertex[num_verts];

	for (int i = 0; i < num_verts; i++){

		node = vert->FirstChild("pos");

		// Read in the vertex data
		vList[i].v[0] = float(atof(node.Element()->Attribute("x")));
		vList[i].v[1] = float(atof(node.Element()->Attribute("y")));
		vList[i].v[2] = float(atof(node.Element()->Attribute("z")));

		node = vert->FirstChild("norm");

		// Read in the normal data
		vList[i].n[0] = float(atof(node.Element()->Attribute("x")));
		vList[i].n[1] = float(atof(node.Element()->Attribute("y")));
		vList[i].n[2] = float(atof(node.Element()->Attribute("z")));	

		// Move to the next one
		vert = vert->NextSiblingElement("vert");
	}
	
	TiXmlElement* faces = model.FirstChild("faces").Element();
	TiXmlNode* face = faces->FirstChild("face");

	num_faces = atoi(faces->Attribute("num"));
	fList = new modelFace[num_faces];

	for (int i = 0; i < num_faces; i++){

		// Get the texture index
		fList[i].tIndex = atoi(face->ToElement()->Attribute("texIndex"));
			

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

		// Can I do this?
		//node = uvParent->FirstChild("v")->Element();
		// Perhaps like this:
		//node = node->NextSiblingElement("v");

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
			collisionModel->addTriangle(vList[fList[i].v[0]].v, vList[fList[i].v[1]].v, vList[fList[i].v[2]].v);
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

int Model::untexturedFaces(){
	if (num_faces == 0) return 0;
	int num = 0;    
	int i = num_faces - 1;
	do{
		if (fList[i].tIndex == 0)
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
	int i = 0, j = 0;
	if (num_faces == 0) return;

	int unTexFaces = untexturedFaces();


	static const u32 TEMP_SIZE = 32768;

	if(displayList)
		free(displayList);
	displayList = NULL;

	u8* tmpDisplayList = (u8*)(memalign(32, TEMP_SIZE));

	if(!tmpDisplayList){
		// PANIC!
		exit(1);
	}

	DCInvalidateRange((void*)tmpDisplayList, TEMP_SIZE);

	GX_BeginDispList(tmpDisplayList, TEMP_SIZE);

	//--DCN: Might need to get rid of this one entirely,
	// or something else, because I'm not sure we're allowed
	// to do a "clearVtxDesc()" and such inside a display list.
	// Render the untextured faces
	if (unTexFaces > 0){

		//
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		//
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);


		GX_Begin(GX_TRIANGLES, GX_VTXFMT_CLR, 3*unTexFaces);
			for (i = 0; i < unTexFaces; i++){

				GX_Position3f32(vList[fList[i].v[0]].v[0],
								vList[fList[i].v[0]].v[1],
								vList[fList[i].v[0]].v[2]);
				GX_Color4u8(fList[i].c[0], fList[i].c[1], fList[i].c[2], 0xff);

				GX_Position3f32(vList[fList[i].v[1]].v[0],
								vList[fList[i].v[1]].v[1],
								vList[fList[i].v[1]].v[2]);
				GX_Color4u8(fList[i].c[3], fList[i].c[4], fList[i].c[5], 0xff);

				GX_Position3f32(vList[fList[i].v[2]].v[0],
								vList[fList[i].v[2]].v[1],
								vList[fList[i].v[2]].v[2]);
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

		for (i = 0; i< num_textures; ++i){

			GX_LoadTexObj(&textureObjs[i], GX_TEXMAP0);

			while((fList[j].tIndex-1) == i && j < num_faces){

				// Fine, rather than being on the outside it'll be on the inside. 

				GX_Begin(GX_TRIANGLES, GX_VTXFMT_TEX_NRM_CLR, 3);

					GX_Position3f32(vList[fList[j].v[0]].v[0],
									vList[fList[j].v[0]].v[1],
									vList[fList[j].v[0]].v[2]);
					GX_Normal3f32(vList[fList[j].v[0]].n[0],
								  vList[fList[j].v[0]].n[1],
								  vList[fList[j].v[0]].n[2]);
					GX_TexCoord2f32(fList[j].uv[0], 1-fList[j].uv[1]);
					GX_Color4u8(0xff,0xff,0xff,0xff);
					//
					GX_Position3f32(vList[fList[j].v[1]].v[0],
									vList[fList[j].v[1]].v[1],
									vList[fList[j].v[1]].v[2]);
					GX_Normal3f32(vList[fList[j].v[1]].n[0],
								  vList[fList[j].v[1]].n[1],
								  vList[fList[j].v[1]].n[2]);
					GX_TexCoord2f32(fList[j].uv[2], 1-fList[j].uv[3]);
					GX_Color4u8(0xff,0xff,0xff,0xff);
					//
					GX_Position3f32(vList[fList[j].v[2]].v[0],
									vList[fList[j].v[2]].v[1],
									vList[fList[j].v[2]].v[2]);
					GX_Normal3f32(vList[fList[j].v[2]].n[0],
								  vList[fList[j].v[2]].n[1],
								  vList[fList[j].v[2]].n[2]);
					GX_TexCoord2f32(fList[j].uv[4], 1-fList[j].uv[5]);
					GX_Color4u8(0xff,0xff,0xff,0xff);

				GX_End();

				++j;

			}
		}
	}

	// Copy the temporary DL into our permanent display list

	//actualDLsize = (GX_EndDispList()+31)&~31;
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
// Function: loadTGATexture
//
//		Loads in a .tga file for the texture.
//		(Exactly the same as LoadFont!)
//
// @pre     The texture to be loaded should exist!
// @post    The textures has been loaded and primed for use
// @param   fileName: The file name and path of the texture to load
// @param   textureObj: The texture object that we are populating
// @return  -
//
//----------------------------------------

void Model::loadTGATexture(const char *fileName, GXTexObj& textureObj){
	const u8 TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};      // Uncompressed TGA header
	u8 TGAcompare[12];               // Used to compare TGA header
	u8 header[6];                    // First 6 useful bytes from the header
	u32 bytesPerPixel;               // Holds number of bytes per pixel used in the TGA file
	u32 bpp;                         // Image color depth in bits per pixel
	u32 imageSize;                   // Used to store the image size when setting aside ram
	u8 *imageData = 0;	             // Image data (Up To 32 Bits)
	u32 temp;                        // Temporary variable
	//u32 type = GL_RGBA;            // Set the default GL mode to RBGA (32 BPP)
	u32 width;                       // Image width
	u32 height;                      // Image height

	FILE *file = fopen(fileName, "rb");            // Open the TGA file

	if( !file || fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||  // Are there 12 bytes to read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 ||                // Does the header match what we want?
		fread(header,1,sizeof(header),file)!=sizeof(header)){               // If so read next 6 header bytes
    
		if (!file){
			fprintf(stderr,"Error: Cannot load %s\n", fileName);
			return;
		}else{
			fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
			fclose(file);                // If anything failed, close the file
			return;
		}
	}

	width  = header[1] * 256 + header[0];        // Determine the TGA width (highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];        // Determine the TGA height (highbyte*256+lowbyte)
    
	 if(width <= 0 || height <= 0 || (header[4]!=24 && header[4]!=32)){ // Is the TGA 24 or 32 bit?
    
		fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
		fclose(file);                    // If anything failed, close the file
		return;
	}

#ifdef TODO_TEXTURE

	bpp = header[4];                           // Grab the TGA's bits per pixel (24 or 32)
	bytesPerPixel = bpp/8;                     // Divide by 8 to get the bytes per pixel
	imageSize = width*height*bytesPerPixel;    // Calculate the memory required for the TGA data
    
	u8* temp = new (std::nothrow) u8 [imageSize];       
	imageData = new (std::nothrow) u8 [width*height*4];   //This will always be 32 bits

	if(!temp || fread(temp, 1, imageSize, file) != imageSize){
		if(temp)
			delete [] temp;

		delete [] imageData;

		fprintf(stderr,"Error: cannot load %s\n", fileName);
		fclose(file);                       // Close the file
		return;
	} 
	fclose (file);                            // Close the file



	if( bytesPerPixel == 4){
		// We have alpha

		for(u32 i = 0; i < imageSize; i += bytesPerPixel){
	                                    
			// We may not need to switch these, if so, just copy them!
			//memcpy(...);
			imageData[i]   = temp[i+2];               
			imageData[i+1] = temp[i+1];  
			imageData[i+2] = temp[i]; 
			imageData[i+3] = temp[i+3]; 
		}
	}else{
		// We do NOT have alpha
		u32 j = 0;
		for(u32 i = 0; i < imageSize; i += bytesPerPixel){
	                                       
			// We may not need to switch these, if so, just copy them!
			//memcpy(...);
			imageData[j]   = temp[i+2];               
			imageData[j+1] = temp[i+1];  
			imageData[j+2] = temp[i]; 
			imageData[j+3] = 0xff; 
	
			j += 4;		// 4 bytes per pixel

		}

	}

#else

	 // This is the original:


	bpp = header[4];                           // Grab the TGA's bits per pixel (24 or 32)
	bytesPerPixel = bpp/8;                     // Divide by 8 to get the bytes per pixel
	imageSize = width*height*bytesPerPixel;    // Calculate the memory required for the TGA data
    
	imageData = new (std::nothrow) u8 [imageSize];        // Reserve memory to hold the TGA data

	if(!imageData ||                        // Does the storage memory exist?
		fread(imageData, 1, imageSize, file)!=imageSize){    // Does the image size match the memory reserved?
   
		if(imageData)        // Was image data loaded
			delete [] imageData;            // If so, release the image data
		fprintf(stderr,"Error: cannot load %s\n", fileName);
		fclose(file);                       // Close the file
		return;
	}
    
	fclose (file);                            // Close the file
	for(u32 i = 0; i < imageSize; i += bytesPerPixel){         // Loop through the image data
											// Swaps the 1st and 3rd bytes ('R'ed and 'B'lue)
		temp=imageData[i];                  // Temporarily store the value at image data 'i'
		imageData[i] = imageData[i + 2];    // Set the 1st byte to the value of the 3rd byte
		imageData[i + 2] = temp;            // Set the 3rd byte to the value in 'temp' (1st byte value)
	}
    
#endif


	GX_InitTexObj(&textureObj, imageData, width, height, 
		// Um! See the "gluBuild2DMipmaps" in the OGl code below. There is no GX_TF_RGB to use!
		(bpp == 24) ? GX_TF_RGBA8 : GX_TF_RGBA8, 
		GX_CLAMP, GX_CLAMP, GX_TRUE );

	// GX_LIN_MIP_LIN is taxing, please see if we can get by with something less
	GX_InitTexObjLOD(&textureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 5, 0, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	// I also made up values for min LOD, max LOD, and the LOD bias.


	/*
	//Create the texture
	glGenTextures(1, textureId);
    
	//Load the texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *textureId);
    
	//Generate the texture
	if (bpp==24){      // Was the TGA 24 bits   
		// If so set the 'type' to GL_RGB
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	}else{    
		// If not, build texture with alpha channel
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	}
    
	//Use nearest filtering, very good
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//*/
	delete[] imageData;
	imageData = NULL;
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

	/*
	Mtx modelview;

	// Rotate
	guMtxIdentity(modelview);
	guMtxRotDeg(modelview, 'x', az);
	guMtxRotDeg(modelview, 'y', ay);
	guMtxRotDeg(modelview, 'z', az);
	//guMtxConcat(modelview, rot, modelview);

	//Translate
	// Maybe this needs to be 
	guMtxTrans(modelview, x, y, z);
	//guMtxTransApply(modelview, modelview, x, y, z);
	//*/
	/*
	Mtx rot, modelview;

	// Rotate
	guMtxIdentity(rot);
	guMtxRotDeg(rot, 'x', az);
	guMtxRotDeg(rot, 'y', ay);
	guMtxRotDeg(rot, 'z', az);
	
	//Translate
	guMtxIdentity(modelview);
	// Maybe this needs to be 
	//guMtxTrans(modelview, x, y, z);
	guMtxTransApply(modelview, modelview, x, y, z);	
	guMtxConcat(modelview, rot, modelview);
	//*/

	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

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
		return 0;
	return list.at(index);
}

Model* ModelList::getModel(const char* name){
	for(u32 i = 0, mListSize = list.size(); i < mListSize; ++i){
		if (strcmp(name, list[i]->getName()) == 0)
			return list[i];
	}
	return 0;
}
