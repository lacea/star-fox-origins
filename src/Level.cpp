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
//					    Level Class							//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#include "Level.h"

// For the lights
#define BIG_NUMBER 32768

extern GXRModeObj *rmode; // From main.cpp

using std::string;

struct litColor{
	GXColor color;		// Color
	GXColor amb;		// Ambient 
	GXColor mat;		// Material 
};
static litColor litcolors[] = {
	{ // Light 0
		{ 0xff, 0xff, 0xff, 0xff }, // Color 0
		{ 0xff, 0xff, 0xff, 0xff }, // Ambient 0
		{ 0xff, 0xff, 0xff, 0xff }  // Material 0
	}
};

// Frames per second
float FPS(){

    static float fFPS      = 0.0f;
    static float fLastTime = 0.0f;
    static int   frames  = 0;
    float fTime = gettime() * 0.001f;
    
    ++frames;
    if( fTime - fLastTime > 1.0f ){
        fFPS      = frames / (fTime - fLastTime);
        fLastTime = fTime;
        frames  = 0;
    }
    return fFPS;

}


//----------------------------------------
//
// Default Constructor
//
// @pre     -
// @post    A new Level Gamestate is created
// @param   -
//----------------------------------------

Level::Level(){
	linePos.x = 0.0f;
	linePos.y = 0.0f;
	linePos.z = 0.0f;
	lineAngle.x = 0.0f;
	lineAngle.y = 0.0f;
	lineAngle.z = 0.0f;

	num_collisions = 0;
	num_shots = 0;

	fogColor.r = 0x00;
	fogColor.g = 0x00;
	fogColor.b = 0x00;
	fogColor.a = 0xff;

	// From old GameState
	camera = new Camera();
	btns = new Buttons();
	modelList = new ModelList();
	objectList = new ObjectList();
	playerShotList = new ObjectList();
	objectTextures = new TextureMgr();
	player = new Player(btns);

}

//----------------------------------------
//
// Destructor
//
// @pre     -
// @post    It's gone!
// @param   -
//----------------------------------------

Level::~Level(){
	clear();

	// From old GameState
	delete camera;
	delete player;
	delete modelList;
	delete objectList;
	delete playerShotList;
	delete objectTextures;
	delete btns;
}


//----------------------------------------
//
// Function: updateSpeedFactor
//
//		How fast we update our movement/frames
//
// @pre     -
// @post    -
// @param   -
// @return  The increment of time that we update out game with
//
//----------------------------------------

float Level::updateSpeedFactor(){
	static u32  currentTime = 0;
	static u32  lastTime = 0;

	currentTime = gettime();
	float ret = (float)(currentTime-lastTime)*0.099;
	lastTime = currentTime;
	return ret;
}

#ifdef TODO

// GAAAAAAAAAAAAAAAAAAAAAAAAAH! Below are three TERRIBLE functions!
#endif

static bool bills(Billboard * bill, Level* lvl, float speedFactor){
	if(bill->pos.z > lvl->camera->pos.z)    {   
		bill->pos.x = static_cast<float>( (rand() % 80) - 40);
		bill->pos.y = static_cast<float>( (rand() % 40) - 20);
		bill->pos.z = lvl->camera->pos.z + (-(rand() % 100) -200);
	}
	return true;
}


bool mShotCallbackL1(Billboard * bill, Level* lvl, float speedFactor){
	
	//a3dssVector3 pos = lvl->player->rMatrix * a3dssVector3(0.0f ,-0.05f, -0.42f);

	guVector temppos;
	guVector adjustPos = {0.0f ,-0.05f, -0.42f};
	guVecMultiply(lvl->player->rotationMtx, &adjustPos, &temppos);
	guVecAdd(&lvl->player->pos, &temppos, &bill->pos);
	
	//a3dssVector3 pos(temppos.x, temppos.y, temppos.z);
	//bill->pos = lvl->player->pos + pos;
	bill->width -= 0.03 * speedFactor;
	bill->height -= 0.03 * speedFactor;
	if (bill->width < 0.0f)    {
		bill->width = 0;
		bill->height = 0;
		return false;
	}
    
	return true;
}

bool explosionCallback(Billboard * bill, Level* lvl, float speedFactor){
	bill->frame += 0.2 * speedFactor;
	if (bill->frame > bill->numFrames)
		return false;
   
	return true;
}


/////////////////////////


//----------------------------------------
//
// Function: loadLevelFile
//
//		Loads the level information from an external file
//
// @pre     The file must exist on the SD card and be properly formatted!
// @post    The level has been loaded and is ready to go.
// @param   xmlFile: The file to load
// @return  bool: true if successful, false if not
//
//----------------------------------------

bool Level::loadLevelFile(string xmlFile){
	
	TiXmlDocument *doc = new TiXmlDocument(xmlFile.c_str());
	if (!doc->LoadFile()){
		fprintf(stderr, "Can't open xml level\n");
		return false;
	}
	
	TiXmlHandle docHandle(doc);
	TiXmlHandle objects = docHandle.FirstChild("objects");

	if(!objects.Element()){
		fprintf(stderr, "No models in the level\n");
		doc->Clear();
		delete doc;
		return false;
	}
	
	Object3D *tempObject = NULL;

	TiXmlNode* object = objects.FirstChild("object").Element();
	for(; object; object = object->NextSibling()){ 
		
		TiXmlAttribute* pAttrib = object->ToElement()->FirstAttribute();
		// Get the name of the model
		string modelName = pAttrib->Value();

		// TEMPORARY -- TAKE OUT OF XML FILE!
		// Maybe the "ship" inside the file is screwing it up?
		if(strcmp("ship", modelName.c_str()) == 0){
			continue;
		}
		
		TiXmlElement* attr = object->FirstChild("position")->ToElement();
		
		if(attr){
			//fprintf(stderr, "attr is here, %s \n", attr->Attribute("x"));
			tempObject = new Object3D(modelList->getModel(modelName.c_str()));
			
			tempObject->pos.x = atof(attr->Attribute("x"));
			tempObject->pos.y = atof(attr->Attribute("y"));
			tempObject->pos.z = atof(attr->Attribute("z"));

			attr = object->FirstChild("rotation")->ToElement();

			if(attr){
				tempObject->ax = atof(attr->Attribute("ax"));
				tempObject->ay = atof(attr->Attribute("ay"));
				tempObject->az = atof(attr->Attribute("az"));
			}else{
				//If there's nothing to load, load in random values
				tempObject->ax = 0.1+static_cast<float>(rand() % 360);
				tempObject->ay = 0.1+static_cast<float>(rand() % 360);
				tempObject->az = 0.1+static_cast<float>(rand() % 360);
			}

			objectList->push(tempObject);

		}
	}

	// Clean up
	doc->Clear();
	delete doc;

	return true;
}

//----------------------------------------
//
// Function: init
//
//		Initialize all the variables for the level,
//		create lights, and prep for rendering.
//
// @pre     -
// @post    The level is ready to play.
// @param   level: The level to load (it's linear, for now)
// @return  bool: true if successful, false if not.
//
//----------------------------------------

bool Level::init(u32 level){

	Object3D* tempObject = NULL;
	Billboard* bill = NULL;
	Model* model = NULL;

	srand(0);

	// Convert the level number into a file name.
	stringstream levelStr(stringstream::in | stringstream::out);
	if(level <= 9){
		levelStr << "0";
	}
	levelStr << level;
	string xmlFile = string(PATH_LEVELS"/level") + levelStr.str() + "/level.xml";

	// Load the level from the file
	loadLevelFile(xmlFile);

    
	// We are going to move all this stuff into the Level xml file.
	// A list of models, textures and such will be listed in there
	// We will load everything in the loadLevelFile function.

	// Here is what it will look like when done: (in loadLevelFile)

	/*
	for(int i = 0; i < extraTexturesNum; i++){
		std::string tex = PATH_TEXTURES;
		tex += loadedFromXMLFileTextures[i];
		objectTextures->pushTexture(tex);
	}
	for(int i = 0; i < modelsNum; i++){
		std::string mod = PATH_MODELS;
		mod += loadedFromXMLFileModels[i];
		model = new Model(mod, true);
		modelList->push(model);
	}
	//*/

	///////////////////////////////////////////
	// This is specific to Level 1 -- PUT INTO XML FILE!

	// Load and push all the textures
	objectTextures->pushTexture(PATH_TEXTURES "flare.png");
	objectTextures->pushTexture(PATH_TEXTURES "particle.png");
	objectTextures->pushTexture(PATH_EXPLOSIONS "explosion01.png");
	objectTextures->pushTexture(PATH_TEXTURES "aim-reticule.png");


	// Load and push all the models
	model = new Model("ship", true);
	modelList->push(model);
	model = new Model("laser", true);
	modelList->push(model);
	model = new Model("asteroid1", true);
	modelList->push(model);
	model = new Model("asteroid2", true);
	modelList->push(model);
	model = new Model("tunnel",true);
	modelList->push(model);

    
	explosionList = new ObjectList();
	billList = new ObjectList();

    
	//add bills to the list
	for (u32 i = 0; i < 200; i++){
	
#ifdef TODO			
#endif		
		//--DCN: This is just terrible. I must change this. BLEH!

		bill = new Billboard(camera, objectTextures->getTexture(1), 
				reinterpret_cast<billFunc>(&bills),
				static_cast<float>((rand() % 80) - 40),
				static_cast<float>((rand() % 40) - 20),
				static_cast<float>(-(rand() % 100)-200), 0.5f, 0.5f);
		//*
		if (!bill)
			return false;
		//*/
		billList->push(bill);

	}



	/*
	///////////////////////////////////////////
	// This is specific to Level 1 -- PUT INTO XML FILE!
	tempObject = new Object3D(modelList->getModel("tunnel"));
	tempObject->destroyable = false;
	tempObject->pos.x = 0.0f;
	tempObject->pos.y = 0.0f;
	tempObject->pos.z = -200.0f;
	tempObject->ax = 0.0f;
	tempObject->ay = 0.0f;
	tempObject->az = 0.0f;

	objectList->push(tempObject);
	//*/


	///////////////////////////////////////////
    
	//Load all player related stuff
	// Set up the player
	player->model = modelList->getModel("ship");
	// Set the player position
	player->pos.x = 0;
	player->pos.y = 0;
	player->pos.z = -13;
	player->destroyable = false;

	/*
	//light
	f32 mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	f32 mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	f32 mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	f32 mat_shininess[] = { 5.0f };
	f32 light_position[] = { 1.0f, 0.0f, 1.0f, 0.0f };    
	f32 mat_emission[] = {1.0, 1.0, 0.0, 1.0};
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//fog
	f32 fogColor[4] = {0.0f, 0.0f, 0.0f, 0.0};
    
	glFogi (GL_FOG_MODE, GL_LINEAR);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_DENSITY, 0.8f);
	glHint (GL_FOG_HINT, GL_NICEST);
	glFogf (GL_FOG_START, 1.0f);
	glFogf (GL_FOG_END, 100.0f);


	//BlendFunction
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE);

	//*/

	// Go here:
	//http://www.google.com/codesearch/p?hl=en#F_nIpjgRyso/trunk/gl2gx/source/gl.c&q=gxcurrentmaterialdiffusecolor%20package:http://gl2gx\.googlecode\.com&sa=N&cd=3&ct=rc&l=228
	//*
	GXLightObj lobj;
	guVector lpos = {1.0f, 0.0f, 1.0f};

	// Infinite position
	//guVecNormalize(&lpos);
	lpos.x *= BIG_NUMBER;
	//lpos.y *= BIG_NUMBER;
	lpos.z *= BIG_NUMBER;
	// Or just:
	// guVector lpos = {BIG_NUMBER, 0.0f, BIG_NUMBER};

	GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
	GX_InitLightColor(&lobj, litcolors[0].color);
	GX_LoadLightObj(&lobj, GX_LIGHT0);
	
	// Set the number of rasterized color channels
	GX_SetNumChans(1);
	// A very simple light
	GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    // Original:
    //GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GX_SetChanAmbColor(GX_COLOR0A0, litcolors[0].amb);
    GX_SetChanMatColor(GX_COLOR0A0, litcolors[0].mat);

	/////////
	// FOG //
	/////////	
	//GX_SetFog(GX_FOG_LIN, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);
	//*/
	return true;
}

//----------------------------------------
//
// Function: clear
//
//		Clear all variables and lists for the level
//
// @pre     -
// @post    The level is now empty.
// @param   -
// @return  -
//
//----------------------------------------

void Level::clear(){
	modelList->clear();
	objectTextures->clear();
	objectList->clear();
	delete billList;
	billList = NULL;
	delete explosionList;
	explosionList = NULL;
}

//-----------------------------------------------------------------------------
// 									RENDERING
//-----------------------------------------------------------------------------

void Level::renderPlayer(Mtx view){

	Mtx mv;
	guMtxCopy(view, mv);
	guMtxTransApply(mv, mv, player->pos.x, player->pos.y, player->pos.z);

	guMtxConcat(mv, player->rotationMtx, mv);

	GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    
	player->model->render(mv);
}
//
void Level::renderBillBoards(Mtx view){
	objectIterator iterator;
	Billboard *bill;

	GX_SetFog(GX_FOG_NONE, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);

	/*
	glDisable(GL_FOG);
	glDepthMask(GL_FALSE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	//*/

	//--DCN: Guessing a bit; the ONE and ONE come from Level::init()
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_AND); 	
	//

	iterator = billList->begin();
	while(iterator != billList->end()){
		bill = static_cast<Billboard *>(*iterator);
		bill->render();
		++iterator;
	}
    
	iterator = explosionList->begin();
	while(iterator != explosionList->end()){
		bill = static_cast<Billboard *>(*iterator);
		bill->render();
		++iterator;
	}
	/*
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	//glDisable(GL_FOG);
	//*/
}



//--DCN: THIS IS WHAT ISN'T WORKING NOW!!!!!

void Level::renderAsteroids(Mtx view){
	Object3D *tempObject;

	//GX_SetFog(GX_FOG_LIN, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);

	objectIterator iterator = objectList->begin();

	// View distance
	static const f32 viewDist = 1000.f;

	do{
		tempObject = static_cast<Object3D *>(*iterator);

		if(tempObject->pos.z < camera->pos.z && tempObject->pos.z > (camera->pos.z - viewDist)){

			Mtx mv, rot;
			guMtxCopy(view, mv);
			
			guMtxTransApply(mv, mv, tempObject->pos.x, tempObject->pos.y, tempObject->pos.z);

			Mtx rotx, roty, rotz;
			guMtxIdentity(rotx);
			guMtxIdentity(roty);
			guMtxIdentity(rotz);
			guMtxRotDeg(rotx, 'x', -tempObject->ax);
			guMtxRotDeg(roty, 'y', tempObject->ay);
			guMtxRotDeg(rotz, 'z', tempObject->az);
			guMtxConcat(rotx, roty, rot);
			guMtxConcat(rot, rotz, rot);

			guMtxConcat(mv, rot, mv);

			//tempObject->model->render(mv);

			//*
			GX_LoadPosMtxImm(mv, GX_PNMTX0);
			//
			GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			//
			GX_ClearVtxDesc();
			GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
			GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);


			const u8 r = 0x00;
			const u8 g = 0x00;
			const u8 b = 0xff;

			GX_Begin(GX_QUADS, GX_VTXFMT_CLR, 24);          

				GX_Position3f32(-1.0f,1.0f,1.0f);       // Top Left of the quad (top)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,1.0f,1.0f);        // Top Right of the quad (top)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,1.0f,-1.0f);       // Bottom Right of the quad (top)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,1.0f,-1.0f);      // Bottom Left of the quad (top)
				GX_Color4u8(r, g, b, 0xff);

				GX_Position3f32(-1.0f,-1.0f,1.0f);      // Top Left of the quad (bottom)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,1.0f);       // Top Right of the quad (bottom)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,-1.0f);      // Bottom Right of the quad (bottom)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,-1.0f,-1.0f);     // Bottom Left of the quad (bottom)
				GX_Color4u8(r, g, b, 0xff);

				GX_Position3f32(-1.0f,1.0f,1.0f);       // Top Left of the quad (front)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,1.0f,1.0f);        // Top Right of the quad (front)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,1.0f);       // Bottom Right of the quad (front)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,-1.0f,1.0f);      // Bottom Left of the quad (front)
				GX_Color4u8(r, g, b, 0xff);

				GX_Position3f32(-1.0f,1.0f,-1.0f);      // Top Left of the quad (back)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,-1.0f,-1.0f);     // Top Right of the quad (back)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,-1.0f);      // Bottom Right of the quad (back)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,1.0f,-1.0f);       // Bottom Left of the quad (back)
				GX_Color4u8(r, g, b, 0xff);

				GX_Position3f32(-1.0f,1.0f,1.0f);       // Top Left of the quad (left)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,1.0f,-1.0f);      // Top Right of the quad (left)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,-1.0f,-1.0f);     // Bottom Right of the quad (left)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(-1.0f,-1.0f,1.0f);      // Bottom Left of the quad (left)
				GX_Color4u8(r, g, b, 0xff);

				GX_Position3f32(1.0f,1.0f,1.0f);        // Top Left of the quad (right)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,1.0f,-1.0f);       // Top Right of the quad (right)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,-1.0f);      // Bottom Right of the quad (right)
				GX_Color4u8(r, g, b, 0xff);
				GX_Position3f32(1.0f,-1.0f,1.0f);       // Bottom Left of the quad (right)
				GX_Color4u8(r, g, b, 0xff);

			GX_End();
			//*/


			// Set the blending function back to its original value
			GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

		}
		++iterator;

	}while(iterator != objectList->end());

}
//
void Level::renderShots(Mtx view){

	objectIterator iterator = playerShotList->begin();
	PlayerShot *temp;
    Mtx mv, rot;

	while(iterator != playerShotList->end()){
		temp = static_cast<PlayerShot *>(*iterator);

		guMtxCopy(view, mv);

		guMtxConcat(mv, temp->rotationMtx, mv);

		guMtxTransApply(mv, mv, temp->pos.x, temp->pos.y, temp->pos.z);

		temp->model->render(mv);

		++iterator;
	}
}

//----------------------------------------
//
// Function: Render
//
//		Clear all variables and lists for the level
//
// @pre     -
// @post    The level is now empty.
// @param   -
// @return  -
//
//----------------------------------------

void Level::Render(){

	//--DCN: I dislike multiplying by random numbers

	camera->pos.x = player->pos.x*(0.8f);
	camera->pos.y = player->pos.y*(0.8f);
	camera->look = camera->pos;
	camera->look.z -= 1.0f;
	/*
	// Should it be done this way?
	camera->look.x = camera->pos.x;
	camera->look.y = camera->pos.y;
	camera->look.z = camera->pos.z - 1;
	//*/
	camera->up.x = cos(((player->ay*0.2f*M_PI)/180.0f)+M_PI/2);
	camera->up.y = sin(((player->ay*0.2f*M_PI)/180.0f)+M_PI/2);

	///////////////////////////////////////////
	Mtx view;
	// Look through the camera
	guLookAt(view, &camera->pos, &camera->up, &camera->look);

	///////////////////////////////////////////

	renderPlayer(view);
	renderShots(view);
	renderAsteroids(view);
	renderBillBoards(view);

	//--------------------------
	// Draw Aiming reticule:

	Mtx modelview;

	// Translate
	guMtxCopy(view, modelview);
	guMtxTransApply(modelview, modelview, player->pos.x, player->pos.y, player->pos.z);

	// Rotate
	guMtxConcat(modelview, player->rotationMtx, modelview);

	// Load the modelview matrix to be our position matrix
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);


	// Render the line helper	
	/*
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_Begin(GX_LINES, GX_VTXFMT_CLR, 2);
		GX_Position3f32(0, 0, 0);
		GX_Color4u8(0xff, 0xff, 0xff, 0xff);
		GX_Position3f32(0, 0, -1000);
		GX_Color4u8(0xff, 0xff, 0xff, 0xff);
	GX_End();
	//*/


	GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);

	// Ignore the light
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
	//GX_InvalidateTexAll();

	GX_LoadTexObj(objectTextures->getTexture(3), GX_TEXMAP0);


	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT_TEX, 4);

		GX_Position3f32(-3.0f,3.0f,-50.0f);
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(3.0f,3.0f,-50.0f);
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(3.0f,-3.0f,-50.0f);
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32(-3.0f,-3.0f,-50.0f);
		GX_TexCoord2f32(0.0f,1.0f);

	GX_End();

	// Set the blending function back to its original value
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	//-------------------------

	// This is what will become our communications 
	// i.e. "All ships report in!"
	/*
	Mtx44 projection;
	guOrtho(projection,0,rmode->fbWidth,0,rmode->efbHeight,0,300);
	GX_LoadProjectionMtx(projection, GX_ORTHOGRAPHIC);

	float fps = FPS();
	PrintText(0, 0, 0, 1.0f, 1.0f, "FPS: %6.1f", fps);
	//PrintText(0, 0, 20, 1.0f, 1.0f, " Particles: %d", billList->getCount());
	PrintText(0, 0, 20, 1.0f, 1.0f, " AX: %7.02f", player->ax);
	PrintText(0, 0, 40, 1.0f, 1.0f, " AY: %7.02f", player->ay);
	PrintText(0, 0, 60, 1.0f, 1.0f, " AZ: %7.02f", player->az);
	PrintText(0, 200, 20, 1.0f, 1.0f, "X: %7.02f", player->pos.x);
	PrintText(0, 200, 40, 1.0f, 1.0f, "Y: %7.02f", player->pos.y);
	PrintText(0, 200, 60, 1.0f, 1.0f, "Z: %7.02f", player->pos.z);
	PrintText(0, 0, 80, 1.0f, 1.0f, " DX: %7.02f", player->dx);
	PrintText(0, 0, 100, 1.0f, 1.0f, " DY: %7.02f", player->dy);
	PrintText(0, 0, 120, 1.0f, 1.0f, " DZ: %7.02f", player->dz);
	PrintText(0, 0, 130, 1.0f, 1.0f, " Asteroids: %d", num_asteroids);
	PrintText(0, 0, 140, 1.0f, 1.0f, " Shots: %d", num_shots);


	// Reset it back to being a perspective projection

	guPerspective(projection, 50, rmode->fbWidth/rmode->efbHeight, 0.1f, 100.0f);
	GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);
	//*/


}

// Logic Functions
void Level::moveBillboards(float speedFactor){

	objectIterator iterator = billList->begin();
	Billboard *bill;
	while (iterator != billList->end()){
		bill = static_cast<Billboard *>(*iterator);
		if (bill->active){
#ifdef TODO
			// This is bad, must change the class!
#endif
			bill->update(this, speedFactor);
			iterator++;
		}
		else
			iterator = billList->erase(iterator);
	}
    
	iterator = explosionList->begin();
	while (iterator != explosionList->end()){
		bill = static_cast<Billboard *>(*iterator);
		if (bill->active){
#ifdef TODO
			// This is bad, must change the class!
#endif
			bill->update(this, speedFactor);
			++iterator;
		}
		else
			iterator = explosionList->erase(iterator);
	}
}

void Level::moveAsteroids(float speedFactor){

	//--DCN: What follows is something just awful. I dislike it very much.

	//object3DIterator iterator = objectList->begin();
	objectIterator iterator = objectList->begin();
	Object3D *tempObject;
	Billboard *bill = 0;
	while (iterator != objectList->end()){
		tempObject = static_cast<Object3D *>(*iterator);
   
		//tempObject->pos.z += tempObject->dz*speedFactor;
		if(!(*iterator)->active){   
			(*iterator)->active = true;
#ifdef TODO			
#endif			
			//--DCN: This is just terrible. I must change this. BLEH!

			bill = new Billboard(camera, objectTextures->getTexture(2),
								 reinterpret_cast<billFunc>(&explosionCallback),
								 tempObject->pos.x, tempObject->pos.y, tempObject->pos.z, 5.0f, 5.0f);


			if (bill){
				bill->build4x4TexCoords();
				explosionList->push(bill);
			}

			tempObject->pos.z -= 400;
		}
		++iterator;
	}
}

void Level::moveShots(float speedFactor){
	static bool pressed = false;
	PlayerShot *temp;
	Billboard *bill = NULL;
	objectIterator iterator = playerShotList->begin();



	u32 numshots = 0;

	while (iterator != playerShotList->end()){
		temp = static_cast<PlayerShot *>(*iterator);

		//--DCN: WARNING! There doesn't seem to be a guVecCopy!
		// Do we need to copy it?! AAAAAAAH!
		//*
		//temp->prevPos.x = temp->pos.x;
		//temp->prevPos.y = temp->pos.y;
		//temp->prevPos.z = temp->pos.z;
		temp->prevPos = temp->pos;

		//temp->pos.z -= temp->speed;

		guVecScale(&temp->direction, &temp->direction, (temp->speed*speedFactor));
		guVecAdd(&temp->pos, &temp->direction, &temp->pos);
		//*/
		/*
		temp->prevPos = temp->pos;
		temp->pos += temp->direction*temp->speed*speedFactor;
		//*/	

		if(temp->pos.z < player->pos.z - 200 || (!(*iterator)->active)){
			iterator = playerShotList->erase(iterator);
		}else{
			++iterator;
		}
	}

	if (btns->shot){
		if (!pressed){

			pressed = true;

			temp = new PlayerShot(modelList->getModel(1));

			// Doing it the more traditional way:
			/*
			Mtx rotx, roty, rotz;
			guMtxIdentity(rotx);
			guMtxIdentity(roty);
			guMtxIdentity(rotz);
			guMtxRotDeg(rotx, 'x', -player->ax);
			guMtxRotDeg(roty, 'y', player->ay);
			guMtxRotDeg(rotz, 'z', player->az);
			guMtxConcat(rotx, roty, temp->rotationMtx);
			guMtxConcat(temp->rotationMtx, rotz, temp->rotationMtx);
			guMtxTransApply(temp->rotationMtx, temp->rotationMtx, 0.0f ,0.05f, -19.3f);
			//*/
			
			// Cheating a little bit: just translating the 
			// already rotated matrix. 
			guMtxTransApply(player->rotationMtx, temp->rotationMtx, 0.0f , 0.05f, -5.3f);
			
			//--DCN: There is no guVecCopy!
			temp->prevPos.x = player->pos.x;
			temp->prevPos.y = player->pos.y;
			temp->prevPos.z = player->pos.z;
			temp->pos.x = player->pos.x;
			temp->pos.y = player->pos.y;
			temp->pos.z = player->pos.z;
			//*/

			guVector dir = {0.0f ,0.0f, -1.0f};
			guVecMultiply(temp->rotationMtx, &dir, &temp->direction);

			//temp->rMatrix = a3dssMatrix3(player->ax, player->ay, player->az);
			//temp->pos = player->pos + temp->rMatrix * a3dssVector3(0.0f ,-0.05f, -1.3f);
			//temp->direction = temp->rMatrix * a3dssVector3(0.0f ,0.0f, -1.0f);

			temp->speed = 0.04f;
			temp->type = TYPE_PLAYERSHOT;
			temp->ax = player->ax;
			temp->ay = player->ay;
			temp->az = player->az;

			playerShotList->push(temp);

#ifdef TODO			
#endif
			//--DCN: This is just terrible. I must change this. BLEH!

			//create the spark in front of the spaceship cannon
			bill = new Billboard(camera, objectTextures->getTexture(0),
								 reinterpret_cast<billFunc>(&mShotCallbackL1),
								 0.0f, 0.0f, 0.0f, 0.6f, 0.3f);
			if (bill){
				billList->push(bill);
			}

		}
	}
	else{
		pressed = false;
	}
}


//----------------------------------------
//
// Function: Logic
//
//		Process all game logic (collisions, position updates, etc)
//
// @pre     -
// @post    All the math has been calculated, positions updated.
// @param   -
// @return  -
//----------------------------------------

void Level::Logic(){
	// YIKES! This makes everything move WAY too fast! Divide for now
	float speedFactor = updateSpeedFactor()/4112;

	// Move player
	player->move(speedFactor);


	//--DCN: TEMPORARY MEASURE: make it loop
	if(player->pos.z < -620){
		player->pos.z = 0;
	}
		

	// Move down the path
	//*
	player->pos.z -= 0.01f*speedFactor;
	camera->pos.z  = player->pos.z+2.6f;
	//*/

	moveShots(speedFactor);
	moveAsteroids(speedFactor);
	moveBillboards(speedFactor);
    
	// Shot line


	//--DCN: There is no guVecCopy!

	linePos.x = player->pos.x;
	linePos.y = player->pos.y;
	linePos.z = player->pos.z;
	// Would this even work?
	//linePos = player->pos;
	lineAngle.x = player->ax;
	lineAngle.y = player->ay;
	lineAngle.z = player->az;

	/*
	linePos = player->pos;
	lineAngle = a3dssVector3(player->ax, player->ay, player->az);
	//*/
    

	// Currently, collision detection DOES NOT WORK!

	//--DCN: I think that the collision model won't work
	// due to the fact that each model is only built once
	// yet has "x, y, and z" in them. We translate the 
	// objects differently now.
	// 

	/////////////////////////
	// Collision detection //
	/////////////////////////

	// Test collisions of shot with asteroids
	objectIterator iterator2;
	/*
	for(objectIterator shotItr = playerShotList->begin(), shotItrEnd = playerShotList->end(); shotItr != shotItrEnd; ++shotItr){
		PlayerShot *shot = static_cast<PlayerShot *>(*shotItr);
		if(!shot->active)
			break;
		for(iterator2 = objectList->begin(); iterator2 != objectList->end(); ++iterator2){
			Object3D *obj = static_cast<Object3D *>(*iterator2);
            
			if(obj->active && obj->model->collisionModel != NULL){
                
				Matrix3D m = TranslateMatrix3D(Vector3D(obj->pos.x, obj->pos.y, obj->pos.z));
				m.rotate(Vector3D(obj->ax, obj->ay, obj->az));
				obj->model->collisionModel->setTransform(m);
				m = TranslateMatrix3D(Vector3D(shot->pos.x, shot->pos.y, shot->pos.z));
				m.rotate(Vector3D(shot->ax, shot->ay, shot->az));
				shot->model->collisionModel->setTransform(m);

				if(obj->model->collisionModel->rayCollision(shot->pos, shot->direction,false,-6.0f, 6.0f)){
					(*shotItr)->active = false;
					if((*iterator2)->destroyable)
						(*iterator2)->active = false;
				}
                
			}
		}
	}
	//*/

	//Test collisions of ship with asteroids
	Matrix3D m = TranslateMatrix3D(Vector3D(player->pos.x, player->pos.y, player->pos.z));
	m.rotate(Vector3D(player->ax, player->ay, player->az));
	player->model->collisionModel->setTransform(m);

	for(iterator2 = objectList->begin(); iterator2 != objectList->end(); ++iterator2){
		if((*iterator2)->active){
			Object3D *obj = static_cast<Object3D *>(*iterator2);
//
//
//THIS is where it all comes crashing down:
			/*
			if(obj->model->collisionModel != NULL){
				Matrix3D m = TranslateMatrix3D(Vector3D(obj->pos.x, obj->pos.y, obj->pos.z));
				m.rotate(Vector3D(obj->ax, obj->ay, obj->az));
				obj->model->collisionModel->setTransform(m);

				if(obj->model->collisionModel->collision(player->model->collisionModel,-1,0,0)){
					if ((*iterator2)->destroyable)
						(*iterator2)->active = false;
				}
			}exit(1);
			//*/
		}
	}

	
}

//----------------------------------------
//
// Function: Input
//
//		Process controller input
//
// @pre     WPAD_ScanPads MUST have been called prior to this!
// @post    All the data has been read in from the Wiimote.
// @param   -
// @return  -
//----------------------------------------

void Level::Input(){

	//--DCN: We are assuming one player here!

	// Would it be better to just grab the whole thing once?
	// i.e.  wiiData = WPAD_Data(WPAD_CHAN_0);
	WPAD_Orientation(WPAD_CHAN_0, &player->orient);
	//WPAD_GForce(WPAD_CHAN_0, &player->gforce);
	WPAD_Accel(WPAD_CHAN_0, &player->accel);


	// After the motion-specific stuff, time to update the buttons!
	btns->update();


}



