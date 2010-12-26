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

#include "Font.h"


Font::Font(){		// Build our font display list

	f32 cx = 0;			// X position of current character
	f32 cy = 0;			// Y position of current character
  

#ifdef TODO_DISPLIST
	// Replace with this:
	/*
	DisplayList* myDL; // Declare globally

	// Create a (large) temporary display list
	DisplayList tmpDL = new DisplayList;

	// Populate the display List
	GX_BeginDispList(tmpDL->list(), tmpDL->size());

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		// ...
		GX_End();

	u32 actualDLsize = GX_EndDispList() + 32;

	myDL = new DisplayList(tmpDL, actualDLsize);
	//*/
#endif


	// For the temporary display lists
	static const u32 TEMP_DISPLIST_SIZE = 128;


    for (s32 j = 0; j < 256; ++j){
    
        cx = f32(j%16)/16.0f;        // X position of current character
        cy = f32(j/16)/16.0f;        // Y position of current character

		u8* tmpDisplayList = (u8*)(memalign(32, TEMP_DISPLIST_SIZE));

		DCInvalidateRange((void*)tmpDisplayList, TEMP_DISPLIST_SIZE);

		GX_BeginDispList(tmpDisplayList, TEMP_DISPLIST_SIZE);

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		GX_Begin(GX_QUADS, GX_VTXFMT_TEX, 4);

#ifdef TODO_FONT
		// I made up values for z! The original was glTexCoord2f
		// Switch to an Ortho view!
#endif
			GX_Position3f32(0, 16, 1);             //Bottom left
			GX_TexCoord2f32(cx, cy+0.0625f); 
			GX_Position3f32(16, 16, 1);            //Bottom right
			GX_TexCoord2f32(cx+0.0625f, cy+0.0625f);   
			GX_Position3f32(16, 0, 1);             //Top right
			GX_TexCoord2f32(cx+0.0625f, cy); 
			GX_Position3f32(0, 0, 1);              //Top left
			GX_TexCoord2f32(cx, cy);         
		GX_End();

		actualDLsize = GX_EndDispList();

		list[j] = (u8*)(memalign(32, actualDLsize));

		DCInvalidateRange((void*)list[j], actualDLsize);
		memcpy(list[j], tmpDisplayList, actualDLsize);
		DCFlushRange((void*)list[j], actualDLsize);

		if(tmpDisplayList)
			free(tmpDisplayList);
		tmpDisplayList = NULL;
    }
}

Font::~Font(){
	s32 f = 255;
	do{
		if(list[f]){
			free(list[f]);
		}
		list[f] = NULL;
		--f;
	}while(f >= 0);
}


bool Font::Load(const char* fileName, Texture& texture){
	return LoadTexture(fileName, texture);
}

void Font::Print(int set, s32 x, s32 y, f32 scalex, f32 scaley, char *fmt, ...){

    if (fmt == NULL)                // If there's no text
        return;                     // Do nothing

    if (set > 1){
        set = 1;
    }

    va_list ap;                     // Pointer to list of arguments
	char text[256];                 // Holds our string
    
    va_start(ap, fmt);              // Parses the string for variables
        vsprintf(text, fmt, ap);    // And converts symbols to actual numbers
    va_end(ap);                     // Results are stored in text
    

	Mtx modelview;
	guMtxTransApply(modelview, modelview, x, y, 0);
	guMtxScale(modelview, scalex, scaley, 1.0f);

    //glListBase(list-32+(128*set));   //?????

	
	// We're using Textures only
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
	//

	// THIS WILL ONLY DO ONE LINE AT A TIME!
	u32 strLen = strlen(text); 
	for(u32 i = 0; i < strLen; i++){
		u8 ascii = text[i];

		// Move it for each letter
		guMtxTransApply(modelview, modelview, 1, 0, 0);

		GX_CallDispList(list[ascii], actualDLsize);
	}
	

	/*
    glDisable(GL_DEPTH_TEST);           // Disables depth testing
    glMatrixMode(GL_PROJECTION);        // Select the projection matrix
    glPushMatrix();                     // Store the projection matrix
    glLoadIdentity();                   // Reset the projection matrix
    glOrtho(0,640, 480,0,-1,1);         // Set up an ortho screen
    glMatrixMode(GL_MODELVIEW);         // Select the modelview matrix
    glPushMatrix();                     // Store the modelview matrix
    glLoadIdentity();                   // Reset the modelview matrix
    glTranslated(x,y,0);                // Position the text (0,0 - Bottom left)
    glListBase(list-32+(128*set));          // Choose the font set (0 or 1)
    glScalef(scalex, scaley, 1.0f);
    glCallLists(strlen(text),GL_BYTE,text); // Write the text to the screen
    glMatrixMode(GL_PROJECTION);                // Select the projection matrix
    glPopMatrix();                              // Restore the old projection matrix
    glMatrixMode(GL_MODELVIEW);                 // Select the modelview matrix
    glPopMatrix();                              // Restore the old projection matrix
    glEnable(GL_DEPTH_TEST);                    // Enables depth testing
	//*/


}
