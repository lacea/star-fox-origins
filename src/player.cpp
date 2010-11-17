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


#define NEW_WII_MOVEMENT


#include "player.h"

Player::Player(Buttons *btns){
    pos = a3dssVector3();
	//GX way:
	//pos.x = 0; pos.y = 0; pos.z = 0;

    ax = ay = az= 0.0f;
    dx = dy = dz= 0.0f;
    mSparkState = lSparkState = rSparkState = 0.0f;
    rMatrix = a3dssMatrix3(ax,ay,az);
	//GX way:
	/*
	guMtxIdentity(rMatrix);
	guMtxRotApply(rMatrix, rMatrix, ax, ay, az);
	//*/
    lives = bombs = 3;
    shield = 1.0f;
    this->btns = btns;
}

Player::Player(Buttons *btns, float x, float y, float z){
    pos = a3dssVector3(x,y,z);
	//GX way:
	//pos.x = x; pos.y = y; pos.z = z;
    ax = ay = az= 0.0f;
    dx = dy = dz= 0.0f;
    mSparkState = lSparkState = rSparkState = 0.0f;
    rMatrix = a3dssMatrix3(ax,ay,az);
	//GX way:
	/*
	guMtxIdentity(rMatrix);
	guMtxRotApply(rMatrix, rMatrix, ax, ay, az);
	//*/
    lives = bombs = 3;
    shield = 1.0f;
    this->btns = btns;
}

Player::~Player(){
    clear();
}

void Player::move(float sfactor){


#ifdef NEW_WII_MOVEMENT
	// Orient our player:

	// x: pitch
	// y: roll
	// z: yaw

	ax = orient.pitch;
	ay = orient.roll;
	az = orient.yaw;


	dy = ax * (MAX_Y_SPEED/MAX_X_ROT);
	dx = -ay * MAX_X_SPEED;

#else

    if (btns->up && !btns->down){
        if ((ax -= ROT_X_SPEED * sfactor) < -MAX_X_ROT)
            ax = -MAX_X_ROT;
        dy = ax * (MAX_Y_SPEED/MAX_X_ROT);
    }
    if (btns->down && !btns->up){
        if ((ax += ROT_X_SPEED * sfactor) > MAX_X_ROT)
            ax = MAX_X_ROT;
        dy = ax * (MAX_Y_SPEED/MAX_X_ROT);
    }
    if (btns->right){
        if ((ay -= ROT_Y_SPEED * sfactor) < -MAX_Y_ROT)
        {
            ay = -MAX_Y_ROT;
        }
        az = ay*(MAX_Z_ROT/MAX_Y_ROT);
        dx = -ay * MAX_X_SPEED;
    }
    if (btns->left){
        if ((ay += ROT_Y_SPEED * sfactor) > MAX_Y_ROT)
        {
            ay = MAX_Y_ROT;
        }
        az = ay*(MAX_Z_ROT/MAX_Y_ROT);
        dx = -ay * MAX_X_SPEED;
    }
    //-----------------------------------------------------
    //no direction (left right) btns pressed, or both directions pressed
    if ((!btns->right && !btns->left) || (btns->right && btns->left)){
        dx = -ay * MAX_X_SPEED;
        //adjust y rot
        if (ay > 0.5){
            if (ay > MAX_Y_ROT / 2){
                ay -= ROT_Y_SPEED * sfactor*1.2f;
            }else if (ay > MAX_Y_ROT / 4){
                ay -= ROT_Y_SPEED * sfactor*0.7f;
            }else{
                ay -= ROT_Y_SPEED * sfactor*0.5f;
            }
            if (ay<0.0f) ay = 0.0f;
        }else if (ay<-0.5) {
            if (-ay > MAX_Y_ROT / 2){
                ay += ROT_Y_SPEED * sfactor*1.2f;
            }else if (ay > MAX_Y_ROT / 4){
                ay += ROT_Y_SPEED * sfactor*0.7f;
            }else{
                ay += ROT_Y_SPEED * sfactor*0.5f;
            }
            if (ay>0.0f) ay = 0.0f;
        }
        else
            ay = 0.0f;
        
        az = ay*(MAX_Z_ROT/MAX_Y_ROT);

    }
    //-----------------------------------------------------
    //no direction (up down) btns pressed, or both directions pressed
    if ((!btns->up && !btns->down) || (btns->up && btns->down)){
        dy = ax * (MAX_Y_SPEED/MAX_X_ROT);
        
        if (ax > 0.5f){
            if (ax > MAX_X_ROT / 2){
                ax -= ROT_X_SPEED * sfactor*0.8f;
            }else if (ax > MAX_X_ROT / 4){
                ax -= ROT_X_SPEED * sfactor*0.6f;
            }else{
                ax -= ROT_X_SPEED * sfactor*0.4f;
            }
            if (ax<0.0f) ax = 0.0f;
        }else if (ax < -0.5f){
            if (-ax > MAX_X_ROT / 2){
                ax += ROT_X_SPEED * sfactor*0.8f;
            }else if (ax > MAX_X_ROT / 4){
                ax += ROT_X_SPEED * sfactor*0.6f;
            }else{
                ax += ROT_X_SPEED * sfactor*0.4f;
            }
            if (ax>0.0f) ax = 0.0f;
        }
        else
            ax = 0.0f;
    }

#endif


	// Update position
    pos.x += dx * sfactor;
    pos.y += dy * sfactor;
    
    if (pos.x >  MAX_X) pos.x = MAX_X;
    if (pos.x < -MAX_X) pos.x = -MAX_X;
    if (pos.y >  MAX_Y) pos.y = MAX_Y;
    if (pos.y < -MAX_Y) pos.y = -MAX_Y;    
    
    update();
}

void Player::update(){
    rMatrix.fromEuler(ax, ay, az);   //update the rotation matrix
	//GX way:
	/*
	guMtxRotDeg(rMatrix, 'x', ax);
	guMtxRotDeg(rMatrix, 'y', ay);
	guMtxRotDeg(rMatrix, 'z', az);
	// Or soon:
	guMtxRotDeg3(rMatrix, ax, ay, az);
	//*/
}

void Player::clear(){

    pos.x = pos.y = pos.z = 0.0f;
    ax = ay = az = 0.0f;
    dx = dy = dz = 0.0f;
    mSparkState = lSparkState = rSparkState = 0;
    lives = bombs = 3;
    shield = 1.0f;
    //only clear model pointer.
    //Model is deleted by ModelList's destructor.
    model = NULL;
}
