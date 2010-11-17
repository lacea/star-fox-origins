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

#ifndef __SF_PLAYERSHOT__
#define __SF_PLAYERSHOT__

#include "baseObject.h"
#include "Model.h"

class PlayerShot : public BaseObject{

public:
    a3dssVector3 prevPos;
    a3dssVector3 direction;
    float ax,ay,az;    //rotation angle
    float speed;    //delta aceleration
    a3dssMatrix3 rMatrix;

    Model* model;

    PlayerShot();
    PlayerShot(float x, float y, float z);
    PlayerShot(Model * model);
    PlayerShot(Model * model, float x, float y, float z);
};

#endif
