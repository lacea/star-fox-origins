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

#include "config.h"

void createDefaultConfig(const char *filename, configData *data){

    TiXmlDocument* doc = new TiXmlDocument();
    TiXmlElement* root = new TiXmlElement("Config");
    //TiXmlElement* video = new TiXmlElement("Video");
    TiXmlElement* btns = new TiXmlElement("Buttons");
    TiXmlElement* leaf = NULL;
    doc->LinkEndChild(new TiXmlDeclaration("1.0", "", ""));
    doc->LinkEndChild(root);
    //root->LinkEndChild(video);
    root->LinkEndChild(btns);
	/*
    leaf = new TiXmlElement("Width");
    leaf->SetAttribute("value", 640);
    video->LinkEndChild(leaf);
    leaf = new TiXmlElement("Height");
    leaf->SetAttribute("value", 480);
    video->LinkEndChild(leaf);
    leaf = new TiXmlElement("Fullscreen");
    leaf->SetAttribute("value", "No");
    video->LinkEndChild(leaf);
	//*/
	/*
    leaf = new TiXmlElement("upBtn");
    leaf->SetAttribute("value", SDLK_UP);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("downBtn");
    leaf->SetAttribute("value", SDLK_DOWN);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("rightBtn");
    leaf->SetAttribute("value", SDLK_RIGHT);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("leftBtn");
    leaf->SetAttribute("value", SDLK_LEFT);
    btns->LinkEndChild(leaf);
	//*/
	
	// Default values for our buttons
	leaf = new TiXmlElement("shotBtn");
	leaf->SetAttribute("value", WPAD_BUTTON_1);
	btns->LinkEndChild(leaf);

	leaf = new TiXmlElement("bombBtn");
	leaf->SetAttribute("value", WPAD_BUTTON_A);
	btns->LinkEndChild(leaf);

	leaf = new TiXmlElement("boostBtn");
	leaf->SetAttribute("value", WPAD_BUTTON_2);
	btns->LinkEndChild(leaf);

	leaf = new TiXmlElement("breakBtn");
	leaf->SetAttribute("value", WPAD_BUTTON_B);
	btns->LinkEndChild(leaf);


    doc->SaveFile(filename);
    doc->Clear();
    delete doc;
    if (!data)
        return;
	/*
    data->width = 640;
    data->height = 480;
    data->fullscreen = 0;
	//*/
	/*
    data->upBtn = SDLK_UP;
    data->downBtn = SDLK_DOWN;
    data->rightBtn = SDLK_RIGHT;
    data->leftBtn = SDLK_LEFT;
	//*/

    data->shotBtn = WPAD_BUTTON_1;
	data->bombBtn = WPAD_BUTTON_A;
	data->boostBtn = WPAD_BUTTON_2;
	data->breakBtn = WPAD_BUTTON_B;

}
//
void saveConfigFile(const char *filename, configData *data){
    if(!filename){
        createDefaultConfig(PATH_GAME "config.xml", 0);
        return;
    }
    if (!data){
        createDefaultConfig(filename, 0);
        return;
    }
    TiXmlDocument* doc = new TiXmlDocument();
    TiXmlElement* root = new TiXmlElement("Config");
    //TiXmlElement* video = new TiXmlElement("Video");
    TiXmlElement* btns = new TiXmlElement("Buttons");
    TiXmlElement* leaf = 0;
    doc->LinkEndChild(new TiXmlDeclaration("1.0", "", ""));
    doc->LinkEndChild(root);
    //root->LinkEndChild(video);
    root->LinkEndChild(btns);

	/*
    leaf = new TiXmlElement("Width");
    leaf->SetAttribute("value", data->width);
    video->LinkEndChild(leaf);
    leaf = new TiXmlElement("Height");
    leaf->SetAttribute("value", data->height);
    video->LinkEndChild(leaf);
    leaf = new TiXmlElement("Fullscreen");
    if(data->fullscreen)
        leaf->SetAttribute("value", "Yes");
    else
        leaf->SetAttribute("value", "No");
    video->LinkEndChild(leaf);
	//*/
	/*
    leaf = new TiXmlElement("upBtn");
    leaf->SetAttribute("value", data->upBtn);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("downBtn");
    leaf->SetAttribute("value", data->downBtn);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("rightBtn");
    leaf->SetAttribute("value", data->rightBtn);
    btns->LinkEndChild(leaf);
    leaf = new TiXmlElement("leftBtn");
    leaf->SetAttribute("value", data->leftBtn);
    btns->LinkEndChild(leaf);
	//*/
	
    leaf = new TiXmlElement("shotBtn");
    leaf->SetAttribute("value", data->shotBtn);
    btns->LinkEndChild(leaf);
	leaf = new TiXmlElement("bombBtn");
    leaf->SetAttribute("value", data->bombBtn);
    btns->LinkEndChild(leaf);
	leaf = new TiXmlElement("boostBtn");
    leaf->SetAttribute("value", data->boostBtn);
    btns->LinkEndChild(leaf);
	leaf = new TiXmlElement("breakBtn");
    leaf->SetAttribute("value", data->breakBtn);
    btns->LinkEndChild(leaf);

    doc->SaveFile(filename);
    doc->Clear();
    delete doc;
}
//
void loadConfigFile(const char *filename, configData *data){
    #define DEFAULT_EXIT {fprintf(stderr, "Invalid configuration: %s, creating default\n", filename); \
                      createDefaultConfig(filename, data); \
                      doc->Clear(); \
                      delete doc; \
                      return;}
    
    if(!filename){
        createDefaultConfig(PATH_GAME "config.xml", data);
        return;
    }
    TiXmlDocument *doc = new TiXmlDocument(filename);
    if (!doc->LoadFile() || !data){
        printf("Creating default config\n");
        createDefaultConfig(filename, data);
        return;
    }
    TiXmlHandle docHandle(doc);
    if(!docHandle.FirstChild("Config").ToElement() ||
       //!docHandle.FirstChild("Config").FirstChild("Video").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Buttons").ToElement() || 
	   /*
       !docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Width").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Height").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Fullscreen").ToElement() ||
	   //*/

	   /*
       !docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("upBtn").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("downBtn").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("rightBtn").ToElement() ||
       !docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("leftBtn").ToElement() ||
	   //*/
       !docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("shotKey").ToElement() )
        DEFAULT_EXIT;
    /*
    if (!docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Width").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->width = atoi(docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Width").ToElement()->Attribute("value"));
    if (!docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Height").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->height = atoi(docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Height").ToElement()->Attribute("value"));
    if (!docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Fullscreen").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    if (strcmp(docHandle.FirstChild("Config").FirstChild("Video").FirstChild("Fullscreen").ToElement()->Attribute("value"), "Yes")==0)
        data->fullscreen = 1;
    else
        data->fullscreen = 0;
	//*/

	/*
    if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("upBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->upBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("upBtn").ToElement()->Attribute("value"));
    if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("downBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->downBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("downBtn").ToElement()->Attribute("value"));
    if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("rightBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->rightBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("rightBtn").ToElement()->Attribute("value"));
    if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("leftBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->leftBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("leftBtn").ToElement()->Attribute("value"));
    //*/

	if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("shotBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->shotBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("shotBtn").ToElement()->Attribute("value"));
    
	if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("bombBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->bombBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("bombBtn").ToElement()->Attribute("value"));
    
	if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("boostBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->boostBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("boostBtn").ToElement()->Attribute("value"));
    
	if (!docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("breakBtn").ToElement()->Attribute("value"))
        DEFAULT_EXIT;
    data->breakBtn = atoi(docHandle.FirstChild("Config").FirstChild("Buttons").FirstChild("breakBtn").ToElement()->Attribute("value"));
    
}
