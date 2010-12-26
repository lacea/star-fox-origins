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

#ifndef __SF_TextureMgr__
#define __SF_TextureMgr__

#include "StdInc.h"
#include <vector>
#include <malloc.h>
#include "libpng/pngu/pngu.h"

class Texture{

public:
	Texture(){data = NULL;}
	~Texture(){
		if(data) 
			free(data);
		data = NULL;
	}

	GXTexObj obj; // Texture object
	u8* data;     // Actual texture data
};

bool LoadTexture(const char* fileName, Texture &texture);

class TextureMgr{

public:
    TextureMgr();
    ~TextureMgr();

    bool pushTexture(const char* fileName);
    GXTexObj* getTexture(s32 index);
	s32 size();
    void clear();

private:

	std::vector<Texture*> vTexture;
};

#endif //TEXTURE_H
