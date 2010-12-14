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

#include "StdInc.h"
#include "math.h"
#include "Buttons.h"
#include "Model.h"
#include "billboard.h"

//#define MOV_SPEED       0.01f
#define ROT_SPEED       1.0f
#define ROT_X_SPEED     0.7f
#define ROT_Y_SPEED     0.7f
#define ROT_Z_SPEED     0.4f
#define MAX_X_SPEED     0.00040f
#define MAX_Y_SPEED     0.006f
#define DEC_X_SPEED     0.06f
#define DEC_Y_SPEED     0.06f
#define MAX_D_SPEED     0.02f
#define MAX_X_ROT       30.0f
#define MAX_Y_ROT       40.0f
#define MAX_Z_ROT       50.0f
#define MAX_X           5.5f
#define MAX_Y           5.5f

class Player : public BaseObject{

public:
	Mtx rotationMtx;
	//a3dssMatrix3 rMatrix;
	Buttons *btns;
	Model *model;
	
	f32 ax, ay, az;
	f32 oldax, olday, oldaz;
	f32 dx, dy, dz;
	f32 shield;
	f32 mSparkState;
	f32 lSparkState;
	f32 rSparkState;
	u16 lives;
	u16 bombs;
	//

	Player(Buttons *btns);
	Player(Buttons *btns, f32 x, f32 y, f32 z);
	~Player();
	void move(float sfactor);
	void update();
	void clear();

	// Wiimote data
	vec3w_t  accel;	  // Wiimote acceleration
	gforce_t gforce;  // Wiimote G-Forces
	orient_t orient;  // Wiimote orientation	

};

#endif
