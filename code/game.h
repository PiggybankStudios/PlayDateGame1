/*
File:   game.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _GAME_H
#define _GAME_H

#define MAIN_FONT_PATH "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define SMALL_FONT_PATH "/System/Fonts/Roobert-10-Bold.pft"

// #define TEXT_WIDTH 112
// #define TEXT_HEIGHT 16
// #define DISPLAY_TEXT "Taylor Robbins!"

struct GameState_t
{
	bool initialized;
	
	PDMenuItem* backgroundMenuItem;
	bool backgroundEnabled;
	
	LCDFont* mainFont;
	LCDFont* smallFont;
	
	SpriteSheet_t testSheet;
	SpriteSheet_t pieSheet;
	
	Texture_t backgroundTexture;
	Texture_t pigTexture;
	
	v2i testSheetFrame;
	
	v2 pigPos;
	v2 pigVel;
	
	bool followingCrank;
	v2 pigOldVel;
	
	bool isInverted;
};

#endif //  _GAME_H
