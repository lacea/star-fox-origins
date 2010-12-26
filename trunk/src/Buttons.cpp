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

#include "Buttons.h"

// TODO: I don't like having so many variables. 
// We can probably cut half of these.
Buttons::Buttons(){
    bup         = WPAD_BUTTON_UP;
    bdown       = WPAD_BUTTON_DOWN;
    bleft       = WPAD_BUTTON_LEFT;
    bright      = WPAD_BUTTON_RIGHT;
    bshot       = WPAD_BUTTON_1;
    bbomb       = WPAD_BUTTON_A;
    bspeedUp    = WPAD_BUTTON_2;
    bspeedDown  = WPAD_BUTTON_B;
    quit        = false;
    pause       = false;
    up          = false;
    down        = false;
    left        = false;
    right       = false;
    shot        = false;
    bomb        = false;
    speedUp     = false;
    speedDown   = false;

    keyState   = NULL;
}

void Buttons::update(){

	// WPAD_ScanPads() has been called already
	u32 btnDown = WPAD_ButtonsDown(0);

	shot  = (btnDown & bshot)  ? true : false;
	bomb  = (btnDown & bbomb)  ? true : false;
	speedUp  = (btnDown & bspeedUp)  ? true : false;
	speedDown  = (btnDown & bspeedDown)  ? true : false;
}

void Buttons::change(int index, s32 newBtn){
    switch (index){
        case CHANGE_BTN_SHOT:  bshot = newBtn; break;
        case CHANGE_BTN_BOMB:  bbomb = newBtn; break;
        case CHANGE_BTN_BOOST: bspeedUp = newBtn; break;
        case CHANGE_BTN_BREAK: bspeedDown = newBtn; break;
		default: break;
    }
}
