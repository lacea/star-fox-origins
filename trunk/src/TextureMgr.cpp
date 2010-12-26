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

#include "TextureMgr.h"

//----------------------------------------
//
// Global Function: LoadTexture
//
//		Loads in a texture from a file and initializes a GXTexture
//
// @pre     -
// @post    The texture has been loaded from the file
// @param   fileName: the file to load
// @param   texture: the Texture struct to populate
// @return  true if successful, false if not
//
//----------------------------------------

bool LoadTexture(const char* fileName, Texture &texture){

	// Open the PNG
	FILE *file = fopen(fileName, "rb");

	if(!file){
		fprintf(stderr, "Error: %s is an invalid texture\n", fileName);
		return false;
	}

	fseek(file, 0, SEEK_END);
	u32 imageSize = ftell(file);
	rewind(file);

	u8* tempData = (u8*)(memalign(32, imageSize));
	if(!tempData){
		fprintf(stderr, "Couldn't allocate data for a texture\n");
		return false;
	}

	fread(tempData, 1, imageSize, file);

	s32 width;
	s32 height;

	texture.data = DecodePNG(tempData, &width, &height, NULL, 0,0);

	GX_InitTexObj(&texture.obj, texture.data, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

	free(tempData);
	fclose(file);

	return true;

}

//----------------------------------------
//
// Default Constructor
//
// @pre     -
// @post    Creates an empty texture holder
// @param   -
//----------------------------------------

TextureMgr::TextureMgr(){}

//----------------------------------------
//
// Destructor
//
// @pre     -
// @post    The texture has been destroyed
// @param   -
//----------------------------------------

TextureMgr::~TextureMgr(){
    while(!vTexture.empty()){
		delete vTexture.back();
        vTexture.pop_back();
    }
}

//----------------------------------------
//
// Function: pushTexture
//
//		Loads in a texture and pushes it onto an array
//
// @pre     -
// @post    The texture has been pushed onto the texture vector array
// @param   fileName: the texture to load and push 
// @return  true if successful, false if not
//
//----------------------------------------

bool TextureMgr::pushTexture(const char* fileName){

	Texture* texture = new Texture;
	bool loaded = LoadTexture(fileName, *texture);
	
	if(loaded){
		vTexture.push_back(texture);
	}

	return loaded;
	
}

//----------------------------------------
//
// Function: getTexture
//
//		Returns a texture at the given index
//
// @pre     index should be within the bounds of our texture vector
// @post    -
// @param   index: which texture to return (by number)
// @return  Pointer to the texture
//
//----------------------------------------

GXTexObj* TextureMgr::getTexture(s32 index){

	if (u32(index) >= vTexture.size())
        return NULL;

    return &(vTexture.at(index)->obj);
}

//----------------------------------------
//
// Function: size
//
//		Get the size of our texture vector
//
// @pre     -
// @post    -
// @param   -
// @return  The size of the texture vector
//
//----------------------------------------

s32 TextureMgr::size(){
    return vTexture.size();
}

//----------------------------------------
//
// Function: clear
//
//		Empty out the texture vector
//
// @pre     -
// @post    vTexture is now empty
// @param   -
// @return  -
//
//----------------------------------------

void TextureMgr::clear(){
    while(!vTexture.empty()){
		delete vTexture.back();
        vTexture.pop_back();
    }
}
