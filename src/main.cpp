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

#include "StdInc.h"
#include "log_console.h"
#include "baseObject.h"
#include "gameState.h"
#include "camera.h"
#include "paths.h"
#include "Font.h"
#include "Model.h"
#include "TextureMgr.h"
#include "Buttons.h"
#include "player.h"
#include "xml/config.h"
#include "Level.h"


// ----------------------------------- //
// -- Take out for finished project -- //

//#define FINISHED_PROJECT
#ifndef FINISHED_PROJECT
	static volatile char buffer[1024*1024*2];
#endif
// ----------------------------------- //


//----------------------//
//   Global variables   //
//----------------------//

// Current level
s32 curLevel = 1;

// This is our current gamestate (replace with something dynamic)
Level* gameState;

// The text that is typed on the screen
Font* screenText;

//----------------------//
// Wii setup variables  //
//----------------------//

#define DEFAULT_FIFO_SIZE (256*1024)

GXRModeObj *rmode = NULL;
Mtx44 projection;
Mtx modelview;
u32 *xfb[2];			// Double framebuffer
s32 currfb;				// Current framebuffer (0 or 1)
static u8 gp_fifo[DEFAULT_FIFO_SIZE] __attribute__((aligned(32)));


//////////////////////////////////////////////////////////////
//					  Function Prototypes					//
//////////////////////////////////////////////////////////////

int main();
static void init();
static void quit_game(int code);
void newExit(void);

//////////////////////////////////////////////////////////////
//							 MAIN							//
//////////////////////////////////////////////////////////////

int main(){

	// Have to make sure we can read the SD card before we use it!
	fatUnmount("sd:/");
	__io_wiisd.shutdown();
	fatMountSimple("sd", &__io_wiisd);
	
	configData config;
	// Load in our configuration
	loadConfigFile(PATH_GAME "config.xml", &config);


	// Initialize our video system
	init();

	// Initialize the console
	log_console_init(rmode, 0, 30, 30, rmode->fbWidth - 200, rmode->xfbHeight - 60);
	log_console_enable_video(true);

	printf("Booting up.\n");			
	
	// Create a new gamestate
    gameState = new Level();

	// Create and load the font
	screenText = new Font();
	screenText->Load(PATH_FONT"plain.png", gameState->font);

	// Change the button configuration
    gameState->btns->change(CHANGE_BTN_SHOT, config.shotBtn);
	gameState->btns->change(CHANGE_BTN_BOMB, config.bombBtn);
	gameState->btns->change(CHANGE_BTN_BOOST, config.boostBtn);
	gameState->btns->change(CHANGE_BTN_BREAK, config.breakBtn);
	
	// Initialize our gamestate
	if (!gameState->init(curLevel))
        quit_game(1);

	// Set the console to invisible
	log_console_enable_video(false);


	// Go through the main loop
    bool playing = true;

	while (playing) {

		WPAD_ScanPads();

		// Do we want to quit?
		if(WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) 
			playing = false;

		// Process Input
		gameState->Input();

		// Game logic
		gameState->Logic();

		// Render
		gameState->Render();

		// Done rendering
		GX_DrawDone();
		currfb ^= 1;
		GX_CopyDisp(xfb[currfb],GX_TRUE);
		VIDEO_SetNextFramebuffer(xfb[currfb]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		
	}


	log_console_enable_video(true);
	// TODO: We should SAVE the game here.
	fprintf(stderr, "\nFinished");
	
	//Quit the game
    quit_game(0);

    // Never reached
    return 0;
}



//////////////////////////////////////////////////////////////
//							Functions						//
//////////////////////////////////////////////////////////////


//----------------------------------------
//
// Function: init
//
//		Initialize the video and Wii controls
//
// @pre     -
// @post    We are ready to roll!
// @param   -
// @return  -
//----------------------------------------

static void init(){

	currfb = 0;

	WPAD_Init();
	VIDEO_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	switch (rmode->viTVMode >> 2){
		case VI_NTSC:	// 480 lines (NTSC 60hz)
		default:		// 480 lines (PAL 60Hz)	
			break;
		case VI_PAL:	// 576 lines (PAL 50hz)
			rmode = &TVPal574IntDfScale;
			rmode->xfbHeight = 480;
			rmode->viYOrigin = (VI_MAX_HEIGHT_PAL - 480)/2;
			rmode->viHeight = 480;
			break;
	}

	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC); //WPAD_FMT_BTNS_ACC_IR
	WPAD_SetVRes(WPAD_CHAN_0,rmode->viWidth,rmode->viHeight);
	//WPAD_SetIdleTimeout(200);

	VIDEO_Configure(rmode);

	xfb[0] = (u32*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = (u32*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_ClearFrameBuffer(rmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(rmode, xfb[1], COLOR_BLACK);
	VIDEO_SetNextFramebuffer(xfb[0]);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	else while (VIDEO_GetNextField()) VIDEO_WaitVSync();

	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	
	const GXColor background = {0, 0, 0, 0xff};
	GX_SetCopyClear(background, GX_MAX_Z24);
 
	// Other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (rmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(xfb[currfb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);

	// Set the width of any lines that we draw
	GX_SetLineWidth(8, GX_TO_EIGHTH);

	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);


	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);

	guPerspective(projection, 45, rmode->fbWidth/rmode->efbHeight, 0.1f, 100.0f);
	GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

	guMtxIdentity(modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	GX_InvVtxCache();
	GX_ClearVtxDesc();
	GX_InvalidateTexAll();
	
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// Texture, normal, and color
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM_CLR, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM_CLR, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM_CLR, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM_CLR, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	// Color only
	GX_SetVtxAttrFmt(GX_VTXFMT_CLR, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_CLR, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	// Texture only
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	// Texture and normal
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_NRM, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	// Texture and color
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_CLR, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_CLR, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT_TEX_CLR, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	//


	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	//Extra, to see what happens.
	//--DCN: TAKE IT OUT!
	//GX_SetCullMode(GX_CULL_NONE);
	//GX_SetCullMode(GX_CULL_FRONT);

}

//
static void quit_game(int code){
    delete screenText;
    delete gameState;
    //newExit();
	exit(code);
}
//
void newExit(void){
	//usleep(3000000);
}