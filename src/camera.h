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
    Camera( float x  = 0.0f, float y  = 0.0f, float z  = 0.0f,
            float lx = 0.0f, float ly = 0.0f, float lz = -1.0f,
            float ux = 0.0f, float uy = 1.0f, float uz = 0.0f);
    void setPos(const float a, const float b, const float c);//{pos = a3dssVector3(a, b, c);}
    inline void setLook(const float a, const float b, const float c);//{look = a3dssVector3(a, b, c);}
    inline void setUp(const float a, const float b, const float c);//{up = a3dssVector3(a, b, c);}
    void update();//{gluLookAt(pos.x, pos.y, pos.z, look.x, look.y, look.z, up.x, up.y, up.z);}
    
    a3dssVector3 pos;
    a3dssVector3 look;   //look vector
    a3dssVector3 up;     //up vector
};

#endif //__CAMERA_H__
