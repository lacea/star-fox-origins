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

#include "billboard.h"

//extern Camera camera;

Billboard::Billboard(Camera *camera, f32 x, f32 y, f32 z, f32 w, f32 h) : BaseObject(x, y, z){
    animated = false;
	dx = dy = dz = 0.0f;
    width = w*0.5f;
	height = h*0.5f;
    active = true;
	texture = 0;
    func = 0;
    stateList = 0;
    frame = 0;
    numFrames = 0;
    this->camera = camera;
}

Billboard::Billboard(Camera *camera, GXTexObj* texture, f32 x, f32 y, f32 z, f32 w, f32 h) : BaseObject(x, y, z){
    animated = false;
    dx = dy = dz = 0.0f;
	width = w*0.5f;
	height = h*0.5f;
    active = true;
    func = 0;
    stateList = 0;
    frame = 0;
    numFrames = 0;
    this->texture = texture;
    this->camera = camera;
}

Billboard::Billboard(Camera *camera, GXTexObj* texture, billFunc func, f32 x, f32 y, f32 z, f32 w, f32 h) : BaseObject(x, y, z){
    animated = false;
    dx = dy = dz = 0.0f;
	width = w*0.5f;
	height = h*0.5f;
    active = true;
    stateList = 0;
    frame = 0;
    numFrames = 0;
    this->texture = texture;
    this->func = func;
    this->camera = camera;
}

void Billboard::setTexture(GXTexObj* texture){
	this->texture = texture;
}

void Billboard::addTexCoord(float stx, float sty, float etx, float ety){
     texCoords tex;
     tex.tx[0] = stx;
     tex.tx[1] = etx;
     tex.ty[0] = sty;
     tex.ty[1] = ety;
     texFrame.push_back(tex);
     animated = true;
     numFrames = texFrame.size();
}

void Billboard::build4x4TexCoords(){
    for(int ty=0;ty<4;ty++)
        for(int tx=0;tx<4;tx++)
            addTexCoord(tx*0.25f, ty*0.25f, tx*0.25f+0.25f, ty*0.25f+0.25f);
}

void Billboard::setCallback(billFunc func){
    this->func = func;
}

void Billboard::setPos(f32 x, f32 y, f32 z){
    pos = a3dssVector3(x,y,z);
}

void Billboard::setSize(f32 w, f32 h){
    width  = w * 0.5f;
	height = h * 0.5f;
}

void Billboard::update(Level* lvl, float speedFactor){
	
#ifdef TODO
	//--DCN: No, this is horrible.
#endif
	//*
    if (func)
        active = func(this, lvl, speedFactor);
	//*/
}

void Billboard::render(){
	// make billboard look to Camera
	a3dssVector3 campos = camera->pos;
	a3dssVector3 target = camera->look;
	a3dssVector3 up = camera->up;
	a3dssVector3 view = target - campos;
	view.normalize();
    
	a3dssVector3 horizontal = up.crossProduct(view);
	horizontal.normalize();
	a3dssVector3 vertical = horizontal.crossProduct(view);
	vertical.normalize();
	horizontal *= width;
	vertical *= height;
    
	
	vertices[0] = pos + horizontal + vertical;
	vertices[1] = pos - horizontal + vertical;
	vertices[2] = pos - horizontal - vertical;
	vertices[3] = pos + horizontal - vertical;
	
    //render the billboard
    
	//glBindTexture(GL_TEXTURE_2D, texture);

	// We are using colors and textures
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	//

	if(animated){
        int currentFrame = int(frame);
        if(currentFrame >= numFrames)
            currentFrame = numFrames-1;

		GX_Begin(GX_QUADS, GX_VTXFMT_TEX_CLR, 4);
			GX_Position3f32(vertices[0].x, vertices[0].y, vertices[0].z);
			GX_TexCoord2f32(texFrame[currentFrame].tx[0], texFrame[currentFrame].ty[1]);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[1].x, vertices[1].y, vertices[1].z);
			GX_TexCoord2f32(texFrame[currentFrame].tx[1], texFrame[currentFrame].ty[1]);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[2].x, vertices[2].y, vertices[2].z);
			GX_TexCoord2f32(texFrame[currentFrame].tx[1], texFrame[currentFrame].ty[0]);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[3].x, vertices[3].y, vertices[3].z);
			GX_TexCoord2f32(texFrame[currentFrame].tx[0], texFrame[currentFrame].ty[0]);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
		GX_End();
		
    }else{
		GX_Begin(GX_QUADS, GX_VTXFMT_TEX_CLR, 4);
			GX_Position3f32(vertices[0].x, vertices[0].y, vertices[0].z);
			GX_TexCoord2f32(0.0f, 1.0f);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[1].x, vertices[1].y, vertices[1].z);
			GX_TexCoord2f32(1.0f, 1.0f);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[2].x, vertices[2].y, vertices[2].z);
			GX_TexCoord2f32(1.0f, 0.0f);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
			GX_Position3f32(vertices[3].x, vertices[3].y, vertices[3].z);
			GX_TexCoord2f32(0.0f, 0.0f);
			GX_Color4u8(0xff, 0xff, 0xff, 0xff);
		GX_End();

    }
}
