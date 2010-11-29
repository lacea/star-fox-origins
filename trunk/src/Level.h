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

// Gamestate for the flying levels, woosh!

#ifndef __SF_LEVEL__
#define __SF_LEVEL__

#include "StdInc.h"
#include <new>
#include <string>
#include <sstream>
#include "billboard.h"
#include "baseObject.h"
#include "camera.h"
#include "playerShot.h"
#include "paths.h"
#include "font.h"
#include "Model.h"
#include "CTexture.h"
#include "Buttons.h"
#include "player.h"
#include "math.h"
#include "object3D.h"
#include "gameState.h"
#include "xml/tinyxml/tinyxml.h"
#include "coldet/coldetimpl.h"

using std::string;
using std::stringstream;

class Level: public GameState{

public:

	Level();
	~Level();


	// The Big Three functions that MUST be implemented
	void Input();
	void Logic();
	void Render();


	// Unique to the Level Gamestate:

	// How fast do we update our movement/frames?
	float updateSpeedFactor();

	// Load in the file with our level objects
	bool loadLevelFile(string xmlFile);

	bool init(u32 level);

	void clear();

	void renderPlayer(Mtx view);
	void renderBillBoards(Mtx view);
	void renderAsteroids(Mtx view);
	void renderShots(Mtx view);
	
	void moveBillboards(float speedFactor);
	void moveAsteroids(float speedFactor);
	void moveShots(float speedFactor);

#ifdef TODO
// Make these private!
private:
#endif

	ObjectList *billList;
	ObjectList *explosionList;
	//GameState *state;
	a3dssVector3 linePos;
	a3dssVector3 lineAngle;

	int num_asteroids;
	int num_collisions;
	int num_shots;

	GXColor fogColor;


	////////////////////////
	// From Old GameState //
	////////////////////////
	Buttons* btns;
    Camera* camera;

    // Texture manager
    CTexture* objectTextures;

    // List of 3D Models
    ModelList* modelList;

    // Lists of all the objects of the level
    ObjectList* objectList;
    ObjectList* playerShotList;
    ObjectList* enemyShotList;
	// The player, obviously
	Player* player;

	// The font that we will be using (possibly use a global font?)
    GXTexObj font;


	/*
	//--DCN: My particular variables

	// Wiimote data
	vec3w_t  accel;	  // Wiimote acceleration
	gforce_t gforce;  // Wiimote G-Forces
	orient_t orient;  // Wiimote orientation	
	//*/
};

#endif
