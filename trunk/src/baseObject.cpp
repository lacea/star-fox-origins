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

#include "baseObject.h"

BaseObject::BaseObject(){
    pos.x = pos.y = pos.z = 0.0f;
    active = true;
    destroyable = true;
}

BaseObject::BaseObject(f32 x, f32 y, f32 z){
	pos.x = x;
	pos.y = y;
    pos.z = z;
    active = true;
    destroyable = true;
}

//---------------------------
// ObjectList

ObjectList::ObjectList(){}

ObjectList::~ObjectList(){
    while(!vector.empty()){
        if(vector.back())
        	delete vector.back();
        vector.pop_back();
    }
}

void ObjectList::push(BaseObject * object){
	vector.push_back(object);
}

void ObjectList::clear(){
    while(!vector.empty()){
        if(vector.back())
        	delete vector.back();
        vector.pop_back();
    }
}

int ObjectList::getCount(){
	return vector.size();
}

objectIterator ObjectList::begin(){
    return vector.begin();
}

objectIterator ObjectList::end(){
    return vector.end();
}

objectIterator ObjectList::erase(objectIterator iterator){
    if(*iterator)
        delete *iterator;
    return vector.erase(iterator);
}
