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

#include "CTexture.h"

//----------------------------------------
//
// Default Constructor
//
// @pre     -
// @post    Creates an empty texture holder
// @param   -
//----------------------------------------

CTexture::CTexture(){}

//----------------------------------------
//
// Destructor
//
// @pre     -
// @post    The texture has been destroyed
// @param   -
//----------------------------------------

CTexture::~CTexture(){
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
//		(Almost exactly the same as LoadFont and Model::loadTGATexture!)
//
// @pre     -
// @post    The texture has been pushed onto the texture vector array
// @param   fileName: the texture to load and push 
// @return  true if successful, false if not
//
//----------------------------------------

bool CTexture::pushTexture(const char * fileName){
	GXTexObj* textureObj = new GXTexObj;
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
			return false;
		}else{
			fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
			fclose(file);                // If anything failed, close the file
			return false;
		}
	}

	width  = header[1] * 256 + header[0];        // Determine the TGA width (highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];        // Determine the TGA height (highbyte*256+lowbyte)
    
	 if(width <= 0 || height <= 0 || (header[4]!=24 && header[4]!=32)){ // Is the TGA 24 or 32 bit?
    
		fprintf(stderr,"Error: %s is an invalid TGA texture\n", fileName);
		fclose(file);                    // If anything failed, close the file
		return false;
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
		return false;
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
    
	imageData = new u8 [imageSize];        // Reserve memory to hold the TGA data

	if(!imageData ||                        // Does the storage memory exist?
		fread(imageData, 1, imageSize, file)!=imageSize){    // Does the image size match the memory reserved?
   
		delete [] imageData;
		fprintf(stderr,"Error: cannot load %s\n", fileName);
		fclose(file);                       // Close the file
		return false;
	}
    
	fclose (file);                            // Close the file
	for(u32 i = 0; i < imageSize; i += bytesPerPixel){         // Loop through the image data
											// Swaps the 1st and 3rd bytes ('R'ed and 'B'lue)
		temp=imageData[i];                  // Temporarily store the value at image data 'i'
		imageData[i] = imageData[i + 2];    // Set the 1st byte to the value of the 3rd byte
		imageData[i + 2] = temp;            // Set the 3rd byte to the value in 'temp' (1st byte value)
	}
    
#endif


	GX_InitTexObj(textureObj, imageData, width, height, 
		// Um! See the "gluBuild2DMipmaps" in the OGl code below. There is no GX_TF_RGB to use!
		(bpp == 24) ? GX_TF_RGBA8 : GX_TF_RGBA8, 
		GX_CLAMP, GX_CLAMP, GX_TRUE );

	// GX_LIN_MIP_LIN is taxing, please see if we can get by with something less
	GX_InitTexObjLOD(textureObj, GX_LIN_MIP_LIN, GX_LINEAR, 0, 5, 0, GX_DISABLE, GX_DISABLE, GX_ANISO_1);
	// I also made up values for min LOD, max LOD, and the LOD bias.

	////////////////////////////////////////////////////////////
	// THIS IS THE ONLY DIFFERENCE BETWEEN THIS FUNCTION AND
	// loadFont and Model::loadTGATexture! AAAAH! The redundancy!
	////////////////////////////////////////////////////////////
	vTexture.push_back(textureObj);


	/*
	//Create the texture
	glGenTextures(1, &textureId);

	//Load the texture
	glBindTexture(GL_TEXTURE_2D, textureId);

	//Generate the texture
	if (bpp==24){      // Was the TGA 24 bits
		// If so set the 'type' to GL_RGB
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	}else {
		// If not, build texture with alpha channel
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	}

	//Use nearest filtering, very good
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

	//push the texture into the texture vector
	vTexture.push_back(textureId);
	//*/ 

	delete [] imageData;

	// Everything was done successfully!
	return true;
	
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

GXTexObj* CTexture::getTexture(int index){
    //if (index<0 || index >= vTexture.size())

	if (u32(index) >= vTexture.size())
        return NULL;

    return vTexture.at(index);
}

//----------------------------------------
//
// Function: getSize [NOT IN USE]
//
//		Get the size of our texture vector
//
// @pre     -
// @post    -
// @param   -
// @return  The size of the texture vector
//
//----------------------------------------
/*
int CTexture::getSize(){
    return vTexture.size();
}
//*/
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

void CTexture::clear(){
    while(!vTexture.empty()){

		delete vTexture.back();

        //glDeleteTextures(1, &vTexture.back());

        vTexture.pop_back();
    }
}
