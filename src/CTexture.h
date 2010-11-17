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
//Original:
//-----------------------------------------------------------------------------
// texture.h
// By martin_bfg10k (martinbfg10k@gmail.com)
// Simple texture manager.
// Only loads uncompressed .tga textures.
//-----------------------------------------------------------------------------

#ifndef __SF_CTEXTURE__
#define __SF_CTEXTURE__

#include "StdInc.h"
#include <vector>

class CTexture{

public:
    CTexture();
    ~CTexture();

    bool pushTexture(const char *);
    GXTexObj* getTexture(int index);
    void clear();

private:

	std::vector<GXTexObj*> vTexture;
};

#endif //TEXTURE_H
