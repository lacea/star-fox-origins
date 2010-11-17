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

#ifndef __SF_OBJECT3D__
#define __SF_OBJECT3D__

#include "baseObject.h"
#include "Model.h"

class Object3D : public BaseObject{

public:
    float ax,ay,az;		// Angle rotation
    float dx,dy,dz;		// Delta aceleration
    float health;		// Health
    Model* model;		// 3D model
    
    Object3D();
    Object3D(float x, float y, float z);
    Object3D(Model* mdl);
    Object3D(Model* mdl, float x, float y, float z);
};

#endif
