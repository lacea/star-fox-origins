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

	f32 ax,ay,az;       // Angle rotation
	f32 dx,dy,dz;       // Delta aceleration
	f32 health;         // Health
	Model* model;       // 3D model

	Object3D();
	//Object3D(f32 x, f32 y, f32 z);
	Object3D(Model* mdl);
	//Object3D(Model* mdl, f32 x, f32 y, f32 z);
};

/*
// From BaseObject

//Define an iterator for select objects from the list
typedef std::list<Object3D* >::iterator object3DIterator;

class Object3DList{

public:
    std::list<Object3D *> vector;
    
    Object3DList();
    ~Object3DList();
    void push(Object3D*);
    void clear();
    int getCount();
    object3DIterator begin();
    object3DIterator end();
    object3DIterator erase(object3DIterator iterator);
};

//*/
#endif
