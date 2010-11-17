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

#include "font.h"


//TODO: Just make this into a class!


// An array of pointers
u8* fontList[256];

// This SHOULD be the exact same size no matter what,
// seeing as how it's just one letter per Display List.
u32 actualFontDLsize;


//--DCN: Exactly the same as Model::loadTGATexture
int LoadFont(char * fileName, GXTexObj &textureObj){
	const u8 TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};      // Uncompressed TGA header
	u8 TGAcompare[12];               // Used to compare TGA header
	u8 header[6];                    // First 6 useful bytes from the header
	u32 bytesPerPixel;               // Holds number of bytes per pixel used in the TGA file
	u32 bpp;                         // Image color depth in bits per pixel
	u32 imageSize;                   // Used to store the image size when setting aside ram
	u8 *imageData = 0;	             // Image data (Up To 32 Bits)
	u32 temp;                        // Temporary variable
	//u32 type = GL_RGBA;            // Set the default GL mode to RBGA (32 BPP)
	u32 width;                       // Image width
	u32 height;                      // Image height

	FILE *file = fopen(fileName, "rb");            // Open the TGA file

	if( !file || fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||  // Are there 12 bytes to read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 ||                // Does the header match what we want?
		fread(header,1,sizeof(header),file)!=sizeof(header)){               // If so read next 6 header bytes
    
		if (!file){
			fprintf(stderr,"Error: Cannot load %s\n", fileName);
			return 0;
		}else{
			fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
			fclose(file);                // If anything failed, close the file
			return 0;
		}
	}

	width  = header[1] * 256 + header[0];        // Determine the TGA width (highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];        // Determine the TGA height (highbyte*256+lowbyte)
    
	 if(width <= 0 || height <= 0 || (header[4]!=24 && header[4]!=32)){ // Is the TGA 24 or 32 bit?
    
		fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
		fclose(file);                    // If anything failed, close the file
		return 0;
	}

#ifdef TODO_TEXTURE

	bpp = header[4];                           // Grab the TGA's bits per pixel (24 or 32)
	bytesPerPixel = bpp/8;                     // Divide by 8 to get the bytes per pixel
	imageSize = width*height*bytesPerPixel;    // Calculate the memory required for the TGA data
    
	u8* temp = new (std::nothrow) u8 [imageSize];       
	imageData = new (std::nothrow) u8 [width*height*4];   //This will always be 32 bits

	if(!temp || fread(temp, 1, imageSize, file) != imageSize){
		if(temp)
			delete [] temp;

		delete [] imageData;

		fprintf(stderr,"Error: cannot load %s\n", fileName);
		fclose(file);                       // Close the file
		return 0;
	} 
	fclose (file);                            // Close the file



	if( bytesPerPixel == 4){
		// We have alpha

		for(u32 i = 0; i < imageSize; i += bytesPerPixel){
	                                    
			// We may not need to switch these, if so, just copy them!
			//memcpy(...);
			imageData[i]   = temp[i+2];               
			imageData[i+1] = temp[i+1];  
			imageData[i+2] = temp[i]; 
			imageData[i+3] = temp[i+3]; 
		}
	}else{
		// We do NOT have alpha
		u32 j = 0;
		for(u32 i = 0; i < imageSize; i += bytesPerPixel){
	                                       
			// We may not need to switch these, if so, just copy them!
			//memcpy(...);
			imageData[j]   = temp[i+2];               
			imageData[j+1] = temp[i+1];  
			imageData[j+2] = temp[i]; 
			imageData[j+3] = 0xff; 
	
			j += 4;		// 4 bytes per pixel

		}

	}

#else

	 // This is the original:


	bpp = header[4];                           // Grab the TGA's bits per pixel (24 or 32)
	bytesPerPixel = bpp/8;                     // Divide by 8 to get the bytes per pixel
	imageSize = width*height*bytesPerPixel;    // Calculate the memory required for the TGA data
    
	imageData = new (std::nothrow) u8 [imageSize];        // Reserve memory to hold the TGA data

	if(!imageData ||                        // Does the storage memory exist?
		fread(imageData, 1, imageSize, file)!=imageSize){    // Does the image size match the memory reserved?
   
		if(imageData)        // Was image data loaded
			delete [] imageData;            // If so, release the image data
		fprintf(stderr,"Error: cannot load %s\n", fileName);
		fclose(file);                       // Close the file
		return 0;
	}
    
	fclose (file);                            // Close the file
	for(u32 i = 0; i < imageSize; i += bytesPerPixel){         // Loop through the image data
											// Swaps the 1st and 3rd bytes ('R'ed and 'B'lue)
		temp=imageData[i];                  // Temporarily store the value at image data 'i'
		imageData[i] = imageData[i + 2];    // Set the 1st byte to the value of the 3rd byte
		imageData[i + 2] = temp;            // Set the 3rd byte to the value in 'temp' (1st byte value)
	}
    
#endif


	GX_InitTexObj(&textureObj, imageData, width, height, 
		// Um! See the "gluBuild2DMipmaps" in the OGl code below. There is no GX_TF_RGB to use!
		(bpp == 24) ? GX_TF_RGBA8 : GX_TF_RGBA8, 
		GX_CLAMP, GX_CLAMP, GX_TRUE );

	// GX_LIN_MIP_LIN is taxing, please see if we can get by with something less
	GX_InitTexObjLOD(&textureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 5, 0, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	// I also made up values for min LOD, max LOD, and the LOD bias.


	/*
    // Build A Texture From The Data
    glGenTextures(1, &image);               // Generate OpenGL texture IDs

    glBindTexture(GL_TEXTURE_2D, image);            // Bind Our Texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       // Linear filtered
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // Linear filtered
    
    if (bpp==24){                        // Was the TGA 24 bits
        type=GL_RGB;                        // If so set the 'type' to GL_RGB
    }

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);
    //*/
    delete [] imageData;
    return 1;            // Texture building went Ok, return true
}



void InitFontSystem(){		// Build our font display list

	float cx = 0;			// X position of current character
	float cy = 0;			// Y position of current character
  

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

	u32 actualFontDLsize = GX_EndDispList() + 32;

	myDL = new DisplayList(tmpDL, actualFontDLsize);
	//*/
#endif


	// For the temporary display lists
	static const u32 TEMP_SIZE = 64;


    for (int j = 0; j < 256; ++j){
    
        cx = float(j%16)/16.0f;        // X position of current character
        cy = float(j/16)/16.0f;        // Y position of current character

#ifdef DIFFERENT_ALLOC

		if(fontList[j])
			free(fontList[j]);

		u8* tmpDisplayList = (u8*)(memalign(32, TEMP_SIZE));
#else
		delete[] fontList[j];	
		u8* tmpDisplayList = new u8[TEMP_SIZE/sizeof(u8)];
#endif

		GX_BeginDispList(tmpDisplayList, TEMP_SIZE);

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

		actualFontDLsize = (GX_EndDispList()+31)&~31;

#ifdef DIFFERENT_ALLOC
		fontList[j] = (u8*)(memalign(32, actualFontDLsize));
#else	
		fontList[j] = new u8[actualFontDLsize / sizeof(u8)]; 
#endif

		memcpy(fontList[j], tmpDisplayList, actualFontDLsize);
		DCFlushRange((void*)fontList[j], actualFontDLsize);


#ifdef DIFFERENT_ALLOC
		if(tmpDisplayList)
			free(tmpDisplayList);
#else	
		delete[] tmpDisplayList;
#endif

    }
}

void KillFontSystem(){
	int f = 255;
	do{
		delete fontList[f];
	}while(f >= 0);


	//delete[] fontList;
	//fontList = NULL;
	
}

void PrintText(int set, s32 x, s32 y, f32 scalex, f32 scaley, char *fmt, ...){

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
	guMtxTrans(modelview, x, y, 0);
	guMtxScale(modelview, scalex, scaley, 1.0f);

    //glListBase(list-32+(128*set));   //?????

	
	// We're using Textures only
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
	//

	u32 strLen = strlen(text); 
	for(u32 i = 0; i < strLen; i++){
		u8 ascii = text[i];

		// Move it for each letter
		guMtxTrans(modelview, 1, 0, 0);

		GX_CallDispList(fontList[ascii], actualFontDLsize);
	}
	
    //glCallLists(strlen(text),GL_BYTE,text); // Write the text to the screen
    
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
