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
//					     Star Fox							//
//					  Wiibrew Project						//
//															//
//					Display List Class						//
//															//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//
// Description:
//
//		Easy wrapper for using display lists.
//
// Example Usage:
/*
	
	DisplayList* myDL; // Declare globally

	// -- Inside your set up function --
	// Create a (large) temporary display list
	DisplayList tmpDL = new DisplayList;

	// Populate the display List
    GX_BeginDispList(tmpDL->list(), tmpDL->size());
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		// ...
		GX_End();

    u32 actualDLsize = GX_EndDispList();

	myDL = new DisplayList(tmpDL, actualDLsize);
	
	// To load another one, we just need to do this:
	DCInvalidateRange(tmpDL->list(), tmpDL->size());
	GX_BeginDispList(tmpDL->list(), tmpDL->size()); 
	//...

	// Then, when we're all done, clean up the temporary
	delete tmpDL;
    
*/
//
#ifndef __SF_DISPLAYLIST__
#define __SF_DISPLAYLIST__

#include "StdInc.h"

#ifdef DIFFERENT_ALLOC
#include <malloc.h>
#endif

using std::memcpy;
///////////////////////////////

// Temporary display list size (used if no size is given)
static const u32 TEMP_DL_SIZE = 32768; //65536;

///////////////////////////////

class DisplayList{

public:

	DisplayList(); // Make a default (maximum size) list
	DisplayList(const DisplayList* dl, u32 newSize); // Copy
	~DisplayList();

	void* list() const;	// Return the list
	u32 size();			// Return the size of the list


private:

	// No one should use these! EVER.
	//DisplayList(const DisplayList& dl);
	//DisplayList& operator=(const DisplayList& dl);

	u32 _size;
	u8* _list;
	
};

#endif

