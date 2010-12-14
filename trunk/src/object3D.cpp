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

#include "object3D.h"

// Baseobject automatically initializes everything to 0.
// Object3D::Object3D() : BaseObject(),
// Or simply:
Object3D::Object3D() : 
	ax(0.0f), ay(0.0f), az(0.0f),
	dx(0.0f), dy(0.0f), dz(0.0f),
	health(0.0f), model(NULL){
	//
}

Object3D::Object3D(Model * mdl) :
	ax(0.0f), ay(0.0f), az(0.0f),
	dx(0.0f), dy(0.0f), dz(0.0f),
	health(0.0f), model(mdl){
	//
}
/*
Object3D::Object3D() : BaseObject(0.0f, 0.0f, 0.0f), 
	ax(0.0f), ay(0.0f), az(0.0f),
	dx(0.0f), dy(0.0f), dz(0.0f),
	health(0.0f), model(NULL){
	//
}

Object3D::Object3D(Model * mdl) : BaseObject(0.0f, 0.0f, 0.0f), 
	ax(0.0f), ay(0.0f), az(0.0f),
	dx(0.0f), dy(0.0f), dz(0.0f),
	health(0.0f), model(mdl){
	//
}
//*/

/*
//---------------------------
// ObjectList

Object3DList::Object3DList(){}

Object3DList::~Object3DList(){
    while(!vector.empty()){
        if(vector.back())
        	delete vector.back();
        vector.pop_back();
    }
}

void Object3DList::push(Object3D * object){
	vector.push_back(object);
}

void Object3DList::clear(){
    while(!vector.empty()){
        if(vector.back())
        	delete vector.back();
        vector.pop_back();
    }
}

int Object3DList::getCount(){
	return vector.size();
}

object3DIterator Object3DList::begin(){
    return vector.begin();
}

object3DIterator Object3DList::end(){
    return vector.end();
}

object3DIterator Object3DList::erase(object3DIterator iterator){
    if(*iterator)
        delete *iterator;
    return vector.erase(iterator);
}

//*/
