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

#ifndef __SF_CONFIG__
#define __SF_CONFIG__

#include "../StdInc.h"
#include "tinyxml/tinyxml.h"

struct configData{
	/*
    int width;
    int height;
    int fullscreen;

    int upBtn;
    int downBtn;
    int rightBtn;
    int leftBtn;
	//*/ 

	//--DCN: Orientation, perhaps?

	int shotBtn;
	int bombBtn;
	int boostBtn;
	int breakBtn;

};

void createDefaultConfig(const char *filename, configData *data);
void saveConfigFile(const char *filename, configData *data);
void loadConfigFile(const char *filename, configData *data);

#endif
