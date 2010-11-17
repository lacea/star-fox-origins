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

#ifndef __SF_STDINC__
#define __SF_STDINC__

// Standard includes

#include <ogcsys.h>
#include <gccore.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <gctypes.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <sdcard/wiisd_io.h>
#include <fat.h>
#include "paths.h"

// We want a different way to allocate our display lists
#define DIFFERENT_ALLOC



// We set up several vertex formats ahead of time:

// Texture, normal, color 
#define GX_VTXFMT_TEX_NRM_CLR	GX_VTXFMT0
// Color Only
#define GX_VTXFMT_CLR			GX_VTXFMT1 
// Texture only
#define GX_VTXFMT_TEX			GX_VTXFMT2 
// Texture and Normal
#define GX_VTXFMT_TEX_NRM		GX_VTXFMT3 
// Texture and Color
#define GX_VTXFMT_TEX_CLR		GX_VTXFMT4 




#endif