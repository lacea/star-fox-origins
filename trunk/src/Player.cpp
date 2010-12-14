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


#include "Player.h"

Player::Player(Buttons *btns){
	pos = a3dssVector3();
	//GX way:
	//pos.x = 0; pos.y = 0; pos.z = 0;

	ax = ay = az = 0.0f;
	oldax = olday = oldaz = 0.0f;
	dx = dy = dz = 0.0f;
	mSparkState = lSparkState = rSparkState = 0.0f;

	guMtxIdentity(rotationMtx);
	//rMatrix = a3dssMatrix3(ax,ay,az);

	lives = bombs = 3;
	shield = 1.0f;
	this->btns = btns;
}

Player::Player(Buttons *btns, f32 x, f32 y, f32 z){
    pos = a3dssVector3(x,y,z);
	//GX way:
	//pos.x = x; pos.y = y; pos.z = z;
    ax = ay = az = 0.0f;
	oldax = olday = oldaz = 0.0f;
    dx = dy = dz = 0.0f;
    mSparkState = lSparkState = rSparkState = 0.0f;

	guMtxIdentity(rotationMtx);
    //rMatrix = a3dssMatrix3(ax,ay,az);

    lives = bombs = 3;
    shield = 1.0f;
    this->btns = btns;
}

Player::~Player(){
    clear();
}

void Player::move(float sfactor){

	// Orient our player:

	// Rotation about the x-axis: roll
	// Rotation about the y-axis: pitch
	// Rotation about the z-axis: yaw




	// Smooth out the rotations

	/*
	stick._x += (_xmouse-stick._x)/6;
	stick._y += (_ymouse-stick._y)/6;
	stick._rotation = (_xmouse-stick._x)/6;
	//*/


	static const f32 ROTATION_THRESHOLD = 3.0f;
	static const f32 ROTATION_CORRECTION = 0.2f;	
	
	f32 tmpax = orient.roll;
	f32 tmpay = orient.pitch;
	f32 tmpaz = orient.yaw; 

	f32 dax = tmpax - ax;
	if(dax > ROTATION_THRESHOLD || dax < -ROTATION_THRESHOLD){
		ax += dax * ROTATION_CORRECTION;
	}

	f32 day = tmpay - ay;
	if(day > ROTATION_THRESHOLD || day < -ROTATION_THRESHOLD){
		ay += day * ROTATION_CORRECTION;
	}

	f32 daz = tmpaz - az;
	if(daz > ROTATION_THRESHOLD || daz < -ROTATION_THRESHOLD){
		az += daz * ROTATION_CORRECTION;
	}
	/*
	// Or maybe this would be faster?
	s32 dax = (s32)(tmpax - ax);
	if(u32(dax) > ROTATION_THRESHOLD){
		ax += dax * ROTATION_CORRECTION;
	}

	s32 day = (s32)(tmpay - ay);
	if(u32(day) > ROTATION_THRESHOLD){
		ay += day * ROTATION_CORRECTION;
	}

	s32 daz = (s32)(tmpaz - az);
	if(u32(daz) > ROTATION_THRESHOLD){
		az += daz * ROTATION_CORRECTION;
	}


	//*/

	/*
	ax = orient.roll;
	ay = orient.pitch;
	az = orient.yaw;

	f32 dax = 0, day = 0, daz = 0;
	f32 tmpdax = ax - oldax;
	if(tmpdax > ROTATION_THRESHOLD || tmpdax < -ROTATION_THRESHOLD){
		dax = tmpdax * 0.5f;
		oldax += dax * ROTATION_CORRECTION;
	}
	ax += dax;

	f32 tmpday = ay - olday;
	if(tmpday > ROTATION_THRESHOLD || tmpday < -ROTATION_THRESHOLD){
		day = tmpday * 0.5f;
		olday += day * ROTATION_CORRECTION;
	}
	ay += day;

	f32 tmpdaz = az - oldaz;
	if(tmpdaz > ROTATION_THRESHOLD || tmpdaz < -ROTATION_THRESHOLD){
		daz = tmpdaz * 0.5f;
		oldaz += daz * ROTATION_CORRECTION;
	}
	az += daz;

	//*/
	/////////////////////////////

	dy = ax * (MAX_Y_SPEED/MAX_X_ROT);
	dx = ay * (MAX_X_SPEED);

	// Update position
    pos.x -= dx * sfactor;
    pos.y -= dy * sfactor;
    
    if (pos.x > MAX_X) pos.x = MAX_X;
    else if (pos.x < -MAX_X) pos.x = -MAX_X;
    if (pos.y > MAX_Y) pos.y = MAX_Y;
    else if (pos.y < -MAX_Y) pos.y = -MAX_Y;    
    
    update();
}

void Player::update(){
    //rMatrix.fromEuler(ax, ay, az);   //update the rotation matrix
	Mtx rotx, roty, rotz;
	guMtxIdentity(rotx);
	guMtxIdentity(roty);
	guMtxIdentity(rotz);
	guMtxRotDeg(rotx, 'x', -ax);
	guMtxRotDeg(roty, 'y', ay);
	guMtxRotDeg(rotz, 'z', az);
	guMtxConcat(rotx, roty, rotationMtx);
	guMtxConcat(rotationMtx, rotz, rotationMtx);
	// Or soon:
	//guMtxRotDeg3(rMatrix, -ax, ay, az);
}

void Player::clear(){

    pos.x = pos.y = pos.z = 0.0f;
    ax = ay = az = 0.0f;
	oldax = olday = oldaz = 0.0f;
    dx = dy = dz = 0.0f;

	guMtxIdentity(rotationMtx);

    mSparkState = lSparkState = rSparkState = 0;
    lives = bombs = 3;
    shield = 1.0f;
    //only clear model pointer.
    //Model is deleted by ModelList's destructor.
    model = NULL;
}
