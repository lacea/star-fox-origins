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

#ifndef __BILLBOARD_H__
#define __BILLBOARD_H__

#include "StdInc.h"
#include <vector>
#include "baseObject.h"
#include "math.h"
#include "camera.h"


class Level;

typedef bool (*billFunc)(const void *, Level* lvl, float speedFactor);
//typedef bool (*billFunc)(const void *, float speedFactor);

typedef struct _texCoords{
    float tx[2];
    float ty[2];
}texCoords, *ptexCoords;

class Billboard : public BaseObject{

    billFunc func;
public:
    float dx,dy,dz;     //delta acceleration
    bool animated;
    f32 width, height;
    int numFrames;
    u32 stateList;
    Camera *camera;
    std::vector<texCoords> texFrame;
    float frame;
    
    Billboard(Camera *camera, f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 1.0f, f32 h = 1.0f);
    //Billboard(Camera *camera, u32 texture, f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 1.0f, f32 h = 1.0f);
    //Billboard(Camera *camera, u32 texture, billFunc func, f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 1.0f, f32 h = 1.0f);
    Billboard(Camera *camera, GXTexObj* texture, f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 1.0f, f32 h = 1.0f);
    Billboard(Camera *camera, GXTexObj* texture, billFunc func, f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 1.0f, f32 h = 1.0f);

	void setTexture(GXTexObj* texture);
    void addTexCoord(float stx, float sty, float etx, float ety);
    void build4x4TexCoords();
    void setCallback(billFunc func);
    void setPos(f32, f32, f32);
    void setSize(f32, f32);
    void update(Level* lvl, float speedFactor);
    void render();
    
private:
    a3dssVector3 vertices[4];
    GXTexObj* texture;
};


#endif  //__BILLBOARD_H__
