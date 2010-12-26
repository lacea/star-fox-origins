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

#include "camera.h"

Camera::Camera(f32 x, f32 y, f32 z, f32 lx, f32 ly, f32 lz, f32 ux, f32 uy, f32 uz){
//:pos({x,y,z}), look({lx,ly,lz}), up({ux,uy,uz}), 
	//*
	pos.x = x;
	pos.y = y;
	pos.z = z;
	look.x = lx;
	look.y = ly;
	look.z = lz;
	up.x = ux;
	up.y = uy;
	up.z = uz;
	//*/

	/*
	pos = a3dssVector3(x,y,z);
    look = a3dssVector3(lx, ly, lz);
    up = a3dssVector3(ux, uy, uz);
	//*/
}

inline void Camera::setPos(const f32 a, const f32 b, const f32 c){

	//*
	pos.x = a;
	pos.y = b;
	pos.z = c;
	//*/

	//pos = a3dssVector3(a, b, c);
}
inline void Camera::setLook(const f32 a, const f32 b, const f32 c){

	//*
	look.x = a;
	look.y = b;
	look.z = c;
	//*/

	//look = a3dssVector3(a, b, c);
}
inline void Camera::setUp(const f32 a, const f32 b, const f32 c){

	//*
	up.x = a;
	up.y = b;
	up.z = c;
	//*/

	//up = a3dssVector3(a, b, c);
}

/*
void Camera::update(){

	// This is going to be a lot more efficient when we change everything to guVectors:
	Mtx modelview;
	guVector post = {pos.x, pos.y, pos.z};
	guVector lookt = {look.x, look.y, look.z};
	guVector upt = {up.x, up.y, up.z};
	
	guLookAt(modelview, &post, &upt, &lookt);
	
	//guLookat(pos, look, up);

}
//*/