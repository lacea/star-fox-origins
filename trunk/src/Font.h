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

#ifndef __SF_FONT__
#define __SF_FONT__

#include "StdInc.h"
#include "TextureMgr.h"

#include <cstdarg>

#include <malloc.h>

//*
class Font{

public:

	Font();
	~Font();

	bool Load(const char* fileName, GXTexObj &textureObj);
	void Print(s32 set, s32 x, s32 y, f32 scalex, f32 scaley, char *fmt, ...);

private:

	// An array of pointers
	u8* list[256];

	// This SHOULD be the exact same size no matter what,
	// seeing as how it's just one letter per Display List.
	u32 actualDLsize;

};
//*/

/*
void InitFontSystem();
void KillFontSystem();
bool LoadFont(const char* fileName, GXTexObj &textureObj);
void PrintText(int set ,s32 x, s32 y, f32 scalex, f32 scaley, char *fmt, ...);
//*/

#endif
