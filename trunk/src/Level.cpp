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
	linePos = a3dssVector3(0.0f ,0.0f, 0.0f);
	lineAngle = a3dssVector3(0.0f ,0.0f, 0.0f);

	num_asteroids = 0;
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
	objectTextures = new CTexture();
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
	
	a3dssVector3 pos = lvl->player->rMatrix * a3dssVector3(0.0f ,-0.05f, -0.42f);
	// GX way:
	//guMtxTransApply(lvl->player->rMatrix, 0, -0.05f, -0.42f);
	
	bill->pos = lvl->player->pos + pos;
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
		// Have to use fprintf for some reason!
		//fprintf(stderr, "Model Name: %s\n", modelName.c_str());	
		
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
	//state = gameState;
	Object3D *tempObject = NULL;
	Billboard *bill = NULL;
	Model* model;

	srand(0);

	// Convert the level number into a file name.
	stringstream levelStr(stringstream::in | stringstream::out);
	if(level <=9)
		levelStr << "0";
	levelStr << level;
	string xmlFile = string(PATH_LEVELS"/level") + levelStr.str() + "/level.xml";
	//string xmlFile = PATH_LEVELS"/level1/level.xml";
	
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
	objectTextures->pushTexture(PATH_TEXTURES "flare.tga");
	objectTextures->pushTexture(PATH_TEXTURES "particle.tga");
	objectTextures->pushTexture(PATH_TEXTURES "explos01.tga");


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

	//dtCreateObject(static_cast<BaseObject *>(tempObject), tempObject->model->shape);
	objectList->push(tempObject);


	///////////////////////////////////////////
    
	//Load all player related stuff
	//setup player
	player->model = modelList->getModel("ship");
	//set player position
	player->pos.x = 0;
	player->pos.y = 0;
	player->pos.z = -13;
	player->destroyable = false;
	//create player collision object
	//dtCreateObject(static_cast<BaseObject *>(state->player), state->player->model->shape);
    

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


	GXLightObj lobj;
	guVector lpos = {1.0f, 0.0f, 1.0f};

	// Infinite position
	guVecNormalize(&lpos);
	lpos.x *= BIG_NUMBER;
	lpos.y *= BIG_NUMBER;
	lpos.z *= BIG_NUMBER;
	// Or just:
	// guVector lpos = {BIG_NUMBER, 0.0f, BIG_NUMBER};

	GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
	GX_InitLightColor(&lobj, litcolors[0].color);
	GX_LoadLightObj(&lobj, GX_LIGHT0);
	
	// Set the number of rasterized color channels
	GX_SetNumChans(1);
	// A very simple light
    GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GX_SetChanAmbColor(GX_COLOR0A0, litcolors[0].amb);
    GX_SetChanMatColor(GX_COLOR0A0, litcolors[0].mat);

	/////////
	// FOG //
	/////////	
	GX_SetFog(GX_FOG_LIN, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);
	//GX_SetFog(GX_FOG_NONE, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);


	// I don't think we want to render just yet.
	//Render();
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
	delete explosionList;
}

//-----------------------------------------------------------------------------
// 									RENDERING
//-----------------------------------------------------------------------------

void Level::renderPlayer(Mtx view){
	//--DCN: I guess we're not using the view matrix (set by the calling function "render()")

	/*
	// I think this is rotating and translating TWICE! Oh no!
	// It's done in state->player->model->render(mv); as well. Maybe I can take it out?!
	Mtx mv, rot;
	//--DCN: Maybe this?
	//guMtxCopy(view, mv);
	guMtxIdentity(mv);
	guMtxTrans(mv, player->pos.x, player->pos.y, player->pos.z );

	guMtxIdentity(rot);
	guMtxRotDeg(rot, 'x', player->az);
	guMtxRotDeg(rot, 'y', player->ay);
	guMtxRotDeg(rot, 'z', player->az);

	guMtxConcat(mv, rot, mv);
	player->model->render(mv);
	//*/
	// ENABLE FOG
	player->model->render();
	// DISABLE FOG

	/*
	glPushMatrix();
	glTranslatef( state->player->pos.x, state->player->pos.y, state->player->pos.z );
	glRotatef( state->player->ax, 1.0f, 0.0f, 0.0f );
	glRotatef( state->player->ay, 0.0f, 1.0f, 0.0f );
	glRotatef( state->player->az, 0.0f, 0.0f, 1.0f );
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_FOG);

	state->player->model->render();

	glDisable(GL_FOG);
	glPopMatrix();
	//*/
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

#ifdef TODO
	glDisable(GL_LIGHTING);
#endif
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
//
void Level::renderAsteroids(Mtx view){
	Object3D *tempObject;

	GX_SetFog(GX_FOG_LIN, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);

	/*
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
	//*/

	objectIterator iterator = objectList->begin();
	num_asteroids = 0;
	while(iterator != objectList->end()){
		tempObject = static_cast<Object3D *>(*iterator);
		//glLoadIdentity( );
		if(tempObject->pos.z<(camera->pos.z) && tempObject->pos.z>(camera->pos.z-500)){

			/*
			Mtx mv, rot;
			//--DCN: Maybe this?
			//guMtxCopy(view, mv);
			guMtxIdentity(mv);
			guMtxTrans(mv, tempObject->pos.x, tempObject->pos.y, tempObject->pos.z );

			guMtxIdentity(rot);
			guMtxRotDeg(rot, 'x', tempObject->az);
			guMtxRotDeg(rot, 'y', tempObject->ay);
			guMtxRotDeg(rot, 'z', tempObject->az);

			guMtxConcat(mv, rot, mv);
			tempObject->model->render(mv);
			//*/
			tempObject->model->render();

			/*
			glPushMatrix();
			glTranslatef(tempObject->pos.x, tempObject->pos.y, tempObject->pos.z );
			glRotatef(tempObject->ax, 1.0f, 0.0f, 0.0f);
			glRotatef(tempObject->ay, 0.0f, 1.0f, 0.0f);
			glRotatef(tempObject->az, 0.0f, 0.0f, 1.0f);
			tempObject->model->render(view);
			glPopMatrix();
			//*/
			++num_asteroids;
		}
		++iterator;
	}
	GX_SetFog(GX_FOG_NONE, 1.0f, 100.0f, 0.0f, 1.0f, fogColor);
	//glDisable(GL_FOG);
}
//
void Level::renderShots(Mtx view){
	objectIterator iterator;
	PlayerShot *temp;
    
	//glEnable(GL_FOG);

#ifdef TODO
	glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
#endif

	iterator = playerShotList->begin();
	num_shots = 0;
    
	while(iterator != playerShotList->end()){
		temp = static_cast<PlayerShot *>(*iterator);

		/*
		Mtx mv, rot;
		//--DCN: Maybe this?
		//guMtxCopy(view, mv);
		guMtxIdentity(mv);
		guMtxTrans(mv, temp->pos.x, temp->pos.y, temp->pos.z );

		guMtxIdentity(rot);
		guMtxRotDeg(rot, 'x', temp->az);
		guMtxRotDeg(rot, 'y', temp->ay);
		guMtxRotDeg(rot, 'z', temp->az);

		guMtxConcat(mv, rot, mv);
		temp->model->render(mv);
		//*/

		temp->model->render();

		/*
		glPushMatrix();
		glTranslatef(temp->pos.x, temp->pos.y, temp->pos.z );
		glRotatef(temp->ax, 1.0f, 0.0f, 0.0f);
		glRotatef(temp->ay, 0.0f, 1.0f, 0.0f);
		glRotatef(temp->az, 0.0f, 0.0f, 1.0f);
		temp->model->render(view);
		glPopMatrix();
		//*/
		++iterator;
		++num_shots;
	}
	/*
	glPopAttrib();
	//*/

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
	camera->up.x = cos(((player->ay*0.2f*M_PI)/180.0f)+M_PI/2);
	camera->up.y = sin(((player->ay*0.2f*M_PI)/180.0f)+M_PI/2);

	///////////////////////////////////////////
	// This is going to be a lot more efficient when we change everything to guVectors:
	Mtx view;
	guVector pos = {camera->pos.x, camera->pos.y, camera->pos.z};
	guVector look = {camera->look.x, camera->look.y, camera->look.z};
	guVector up = {camera->up.x, camera->up.y, camera->up.z};
	
	guLookAt(view, &pos, &up, &look);
	GX_LoadPosMtxImm(view, GX_PNMTX0);	
	///////////////////////////////////////////

	//--DCN: You know what would be CRAZY?!?!
	// Combining all these one-time-use functions into one!
	// I told you, I'm CRAZY! 
	renderPlayer(view);
	renderShots(view);
	renderAsteroids(view);
	renderBillBoards(view);
    
	//--------------------------
	// Render the line helper
	Mtx modelview;
	Mtx rot;		// Rotational matrix

	// Translate
	guMtxIdentity(modelview);
	//guMtxTransApply(modelview, modelview, player->pos.x, player->pos.y, player->pos.z);

	//--DCN: For some reason, the z value is a REDICULOUSLY huge number
	guMtxTransApply(modelview, modelview, player->pos.x/2, player->pos.y/2, - 20);

	
	//fprintf(stderr, "[Pos] x: %f, y: %f, z: %f\n", player->pos.x, player->pos.y, player->pos.z);
	// Rotate
	guMtxIdentity(rot);

	guMtxRotDeg(rot, 'x', player->ax);
	guMtxRotDeg(rot, 'y', player->ay);
	guMtxRotDeg(rot, 'z', player->az);

	// Combine the matrices
	guMtxConcat(modelview, rot, modelview);

	// Load the modelview matrix to be our position matrix
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_Begin(GX_LINES, GX_VTXFMT_CLR, 2);
		GX_Position3f32(0, 0, 0);
		GX_Color4u8(0xff, 0xff, 0xff, 0xff);
		GX_Position3f32(0, 0, -1000);
		GX_Color4u8(0xff, 0xff, 0xff, 0xff);
	GX_End();

	//////////////////////
	// TESTING
	//////////////////////
	//*
	// Why does this come out GREEN!?!?!
		u8 r = 0x00;
		u8 g = 0x00;
		u8 b = 0xFF;

		//guMtxIdentity(modelview);
		//guMtxTransApply(modelview, modelview, 0.0f,0.0f,-7.0f);
		//GX_LoadPosMtxImm(modelview, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT_CLR, 24);		

			GX_Position3f32(-1.0f,1.0f,1.0f);	// Top Left of the quad (top)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,1.0f,1.0f);	// Top Right of the quad (top)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,1.0f,-1.0f);	// Bottom Right of the quad (top)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,1.0f,-1.0f);	// Bottom Left of the quad (top)
			GX_Color4u8(r, g, b, 0xff);

			GX_Position3f32(-1.0f,-1.0f,1.0f);	// Top Left of the quad (bottom)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,1.0f);	// Top Right of the quad (bottom)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,-1.0f);	// Bottom Right of the quad (bottom)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Bottom Left of the quad (bottom)
			GX_Color4u8(r, g, b, 0xff);

			GX_Position3f32(-1.0f,1.0f,1.0f);	// Top Left of the quad (front)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,-1.0f,1.0f);	// Top Right of the quad (front)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,1.0f);	// Bottom Right of the quad (front)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,1.0f,1.0f);	// Bottom Left of the quad (front)
			GX_Color4u8(r, g, b, 0xff);

			GX_Position3f32(-1.0f,1.0f,-1.0f);	// Top Left of the quad (back)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Top Right of the quad (back)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,-1.0f);	// Bottom Right of the quad (back)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,1.0f,-1.0f);	// Bottom Left of the quad (back)
			GX_Color4u8(r, g, b, 0xff);

			GX_Position3f32(-1.0f,1.0f,1.0f);	// Top Left of the quad (left)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,1.0f,-1.0f);	// Top Right of the quad (back)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Bottom Right of the quad (back)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(-1.0f,-1.0f,1.0f);	// Bottom Left of the quad (back)
			GX_Color4u8(r, g, b, 0xff);

			GX_Position3f32(1.0f,1.0f,1.0f);	// Top Left of the quad (right)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,1.0f,-1.0f);	// Top Right of the quad (right)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,-1.0f);	// Bottom Right of the quad (right)
			GX_Color4u8(r, g, b, 0xff);
			GX_Position3f32(1.0f,-1.0f,1.0f);	// Bottom Left of the quad (right)
			GX_Color4u8(r, g, b, 0xff);

		GX_End();
	//*/
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
	Billboard *bill = 0;
	objectIterator iterator = playerShotList->begin();
	while (iterator != playerShotList->end()){
		temp = static_cast<PlayerShot *>(*iterator);
		temp->prevPos = temp->pos;
		temp->pos += temp->direction*temp->speed*speedFactor;
	
		if(temp->pos.z < player->pos.z - 50){
			iterator = playerShotList->erase(iterator);
		}else if (!(*iterator)->active) {
			iterator = playerShotList->erase(iterator);
		}
		else{
			++iterator;
		}
	}

	if (btns->shot){
		if (!pressed){
			pressed = true;

			temp = new (std::nothrow) PlayerShot(modelList->getModel(1));
			temp->rMatrix = a3dssMatrix3(player->ax, player->ay, player->az);
			temp->pos = player->pos + temp->rMatrix * a3dssVector3(0.0f ,-0.05f, -1.3f);
			// GX way:
			/*
			guMtxRotDeg(temp->rMatrix, temp->rMatrix, 'x', player->ax);
			guMtxRotDeg(temp->rMatrix, temp->rMatrix, 'y', player->ay);
			guMtxRotDeg(temp->rMatrix, temp->rMatrix, 'z', player->az);
			// Or soon:
			//guMtxRotDeg3(temp->rMatrix, temp->rMatrix, player->ax, player->ay, player->az);

			guMtxTransApply(temp->rMatrix, 0.0f ,-0.05f, -1.3f);
			//*/

			temp->prevPos = temp->pos;
			temp->direction = temp->rMatrix * a3dssVector3(0.0f ,0.0f, -1.0f);
			temp->speed = 0.7f;
			if (!temp)
				return;
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
	float speedFactor = updateSpeedFactor();  //update speed according with the cpu speed

	//--DCN: Take out this function, it's bad and horrible and bad!

	// YIKES! This makes everything move WAY too fast! Divide for now
	player->move(speedFactor/1028);

	// Move player
	player->pos.z-= 0.03f*speedFactor;;
	camera->pos.z = player->pos.z+2.6f;
    
	moveShots(speedFactor);
	moveAsteroids(speedFactor);
	moveBillboards(speedFactor);
    
	// Shot line
	linePos = player->pos;// + player->rMatrix * a3dssVector3(0.0f ,-0.2f, -6.0f);
	lineAngle = a3dssVector3(player->ax, player->ay, player->az);
    

	// Currently, collision detection DOES NOT WORK!
	/////////////////////////
	// Collision detection //
	/////////////////////////
	//*
	// Test collisions of shot with asteroids
	objectIterator iterator2; // = objectList->begin();

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

				if(obj->model->collisionModel->rayCollision(shot->pos.v, shot->direction.v,false,-6.0f, 6.0f)){
					(*shotItr)->active = false;
					if((*iterator2)->destroyable)
						(*iterator2)->active = false;
				}
                
			}
		}
	}

	//--DCN: 
	/*
	The objectIterator iterates on the BASE class. 
	I think that, even though we cast it to a derived class 
	(Object3D), it isn't recognizing the added variables 
	in the derived class. Not sure though.
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



