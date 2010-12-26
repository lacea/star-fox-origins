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

#ifndef __SF_BASEOBJECT__
#define __SF_BASEOBJECT__

#include "StdInc.h"
#include <vector>
#include <list>
#include "math.h"

typedef enum {
    TYPE_NONE = 0,
    TYPE_PLAYER,
    TYPE_PLAYERSHOT,
    TYPE_ASTEROID
} types;

class BaseObject {

public:
	guVector pos;
    //a3dssVector3 pos;
    types type;
    bool destroyable;
    bool active;

    BaseObject();
    BaseObject(f32 x, f32 y, f32 z);
    
};

//--DCN: THIS must be rewritten. Some objects have models, some don't.
// The models are not available with a simple static_cast done to
// the objectIterator later in the program.

// Probably several objectIterators will be needed.
//		- Player lasers
//		- Asteroids (or any 3D object)
//		- Billboards (2D objects)

//Define an iterator for select objects from the list
typedef std::list<BaseObject* >::iterator objectIterator;

// Typedef for my Compare Function used for sort.
typedef int (*cmpf)(const void *, const void *);

class ObjectList{
	void QSort(int l, int r);
    cmpf Compare;   //compare function for sort
public:
    std::list<BaseObject *> vector;
    
    ObjectList();
    ~ObjectList();
    void push(BaseObject*);
    int sort(int (*compareFunction)(const void*, const void*));
    void clear();
    int getCount();
    objectIterator begin();
    objectIterator end();
    objectIterator erase(objectIterator iterator);
};

#endif
