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

#include "playerShot.h"

//------------
// PlayerShot 
//------------

PlayerShot::PlayerShot() : BaseObject(0.0f, 0.0f, 0.0f){
    direction.x = direction.y = 0.0f; direction.z = -1.0f;
    prevPos.x = prevPos.y = prevPos.z = 0.0f;
    ax = ay = az = 0.0f;
    model = 0;
	guMtxIdentity(rotationMtx);
}

PlayerShot::PlayerShot(f32 x, f32 y, f32 z) : BaseObject(x, y, z){
    direction.x = direction.y = 0.0f; direction.z = -1.0f;
    prevPos.x = prevPos.y = prevPos.z = 0.0f;
    ax = ay = az= 0.0f;
    model = 0;
	guMtxIdentity(rotationMtx);
}

PlayerShot::PlayerShot(Model * model) : BaseObject(0.0f, 0.0f, 0.0f){
    direction.x = direction.y = 0.0f; direction.z = -1.0f;
    prevPos.x = prevPos.y = prevPos.z = 0.0f;
    ax = ay = az = 0.0f;
    this->model = model;
	guMtxIdentity(rotationMtx);
}

PlayerShot::PlayerShot(Model * model, f32 x, f32 y, f32 z) : BaseObject(x, y, z){
    direction.x = direction.y = 0.0f; direction.z = -1.0f;
    prevPos.x = prevPos.y = prevPos.z = 0.0f;
    ax = ay = az = 0.0f;
    this->model = model;
	guMtxIdentity(rotationMtx);
}
