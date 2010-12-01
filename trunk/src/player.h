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

#ifndef __SF_PLAYER__
#define __SF_PLAYER__

#include "math.h"
#include "Buttons.h"
#include "Model.h"
#include "billboard.h"

//#define MOV_SPEED       0.01f
#define ROT_SPEED       1.0f
#define ROT_X_SPEED     0.7f
#define ROT_Y_SPEED     0.7f
#define ROT_Z_SPEED     0.4f
#define MAX_X_SPEED     0.0020f
#define MAX_Y_SPEED     0.06f
#define DEC_X_SPEED     0.06f
#define DEC_Y_SPEED     0.06f
#define MAX_D_SPEED     0.02f
#define MAX_X_ROT       30.0f
#define MAX_Y_ROT       40.0f
#define MAX_Z_ROT       50.0f
#define MAX_X           6.0f
#define MAX_Y           6.0f

class Player : public BaseObject{

public:
    float ax, ay, az;
    float dx, dy, dz;
    a3dssMatrix3 rMatrix;
    Buttons *btns;
    u16  lives;
    u16  bombs;
    float shield;
    Model *model;
    float mSparkState;
    float lSparkState;
    float rSparkState;
	
	//

    Player(Buttons *btns);
    Player(Buttons *btns, float x, float y, float z);
    ~Player();
    void move(float sfactor);
    void update();
    void clear();



	//--DCN: My particular variables

	// Wiimote data
	vec3w_t  accel;	  // Wiimote acceleration
	gforce_t gforce;  // Wiimote G-Forces
	orient_t orient;  // Wiimote orientation	

};

#endif
