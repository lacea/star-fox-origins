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

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//															//
//					 Star Fox Origins						//
//					     									//
//					Display List Class						//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#include "DisplayList.h"

//----------------------------------------
//
// Default Constructor
//
// @pre     -
// @post    Creates large display list (perfect for temporaries)
// @param   -
//----------------------------------------

DisplayList::DisplayList(): 
		_size(TEMP_DL_SIZE){
	
	_list = (u8*)(memalign(32, _size));

	// Get the list primed for GX_BeginDispList
	DCInvalidateRange((void*)_list, _size);

	// Set it all to 0
	memset(_list, 0, _size);
 
}

//----------------------------------------
//
// Copy Constructor
//
// @pre     A list (usually a temporary, see above) already exists
// @post    The display list has been copied, with a smaller size
// @param   dl: The display list to copy
// @param   newSize: The amount of space that dl actually occupies
//----------------------------------------

DisplayList::DisplayList(const DisplayList* dl, u32 newSize):
		_size((newSize + 31)&~31){    // Display lists need to be at least 32 bytes more than needed

	_list = (u8*)(memalign(32, _size));

	DCInvalidateRange((void*)_list, _size);
    memcpy(_list, dl->list(), _size);
    DCFlushRange((void*)_list, _size);
}
//----------------------------------------
//
// Destructor
//
// @pre     -
// @post    The list is deleted.
// @param   -
//----------------------------------------

DisplayList::~DisplayList(){
	if(_list)
		free(_list);
	_list = NULL;
}

//----------------------------------------
//
// Function: list
//
//    Returns the list (NOTE: This breaks encapsulation)
//
// @pre     -
// @post    -
// @param   -
// @return  Pointer to the display list
//
//----------------------------------------

void* DisplayList::list() const{
	return ((void*)(_list));
}

//----------------------------------------
//
// Function: size
//
//    Returns the size of the list
//
// @pre     -
// @post    -
// @param   -
// @return  Size of the list in bytes
//
//----------------------------------------

u32 DisplayList::size(){
	return _size;
}

// In case we want to overwrite what we have?
/*
DisplayList::Copy(const DisplayList& dl, u32 newSize){
	if (this == &dl)	// You should never do that!
		return *this;

	delete[] list;
	
	_size = newSize;	
	list = new u8[newSize/sizeof(u8)]; 
	
    memcpy((void*)list, (void*)dl.list(), newSize);
    DCFlushRange((void*)list, newSize);
}
//*/

/*

// We should never call this, so it's useless.

DisplayList& DisplayList::operator=(const DisplayList& dl){
	if (this == &dl)
		return *this;

	delete[] list;
	_size = dl.size();	
	list = new u8[_size/sizeof(u8)]; 
	
    memcpy((void*)list, (void*)dl.list(), _size);
    DCFlushRange((void*)list, _size);
	           

	return *this;
}
//*/

