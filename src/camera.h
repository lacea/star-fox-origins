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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "StdInc.h"
#include "math.h"

class Camera{
public:
    Camera( f32 x  = 0.0f, f32 y  = 0.0f, f32 z  = 0.0f,
            f32 lx = 0.0f, f32 ly = 0.0f, f32 lz = -1.0f,
            f32 ux = 0.0f, f32 uy = 1.0f, f32 uz = 0.0f);
    inline void setPos(const f32 a, const f32 b, const f32 c);
    inline void setLook(const f32 a, const f32 b, const f32 c);
    inline void setUp(const f32 a, const f32 b, const f32 c);
	inline void set(const guVector p, const guVector l, const guVector u);
    void update();
    
    guVector pos;    // Position vector
    guVector look;   // Look vector
    guVector up;     // Up vector

};

#endif //__CAMERA_H__
