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

#ifndef __SF_BUTTONS__
#define __SF_BUTTONS__

#include "StdInc.h"
#include <cstdio>

/*
#define CHANGE_KEY_UP 1
#define CHANGE_KEY_DOWN 2
#define CHANGE_KEY_LEFT 3
#define CHANGE_KEY_RIGHT 4
#define CHANGE_KEY_SHOT 5
//*/

#define CHANGE_BTN_SHOT 1
#define CHANGE_BTN_BOMB 2
#define CHANGE_BTN_BOOST 3
#define CHANGE_BTN_BREAK 4

class Buttons {

//class Keys {

    s32 bup;
    s32 bdown;
    s32 bleft;
    s32 bright;
    s32 bshot;
    s32 bbomb;
    s32 bspeedUp;
    s32 bspeedDown;
    u8 *keyState;
    
public:
    //system keys
    bool quit;
    bool pause;
    
    //player keys
    bool   up;
    bool   down;
    bool   left;
    bool   right;
    bool   shot;
    bool   bomb;
    bool   speedUp;
    bool   speedDown;
    
    Buttons();

    void update();
    void change(s32 index, s32 newKey);
};

#endif

