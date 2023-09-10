/*
File:   game.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _GAME_H
#define _GAME_H

#define MAIN_FONT_PATH  "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define SMALL_FONT_PATH "/System/Fonts/Roobert-10-Bold.pft"
#define GAME_FONT_PATH  "Resources/Fonts/MVP"

#define GAME_RETURN_TO_MENU_HOLD_B_TIME 1000

// #define TEXT_WIDTH 112
// #define TEXT_HEIGHT 16
// #define DISPLAY_TEXT "Taylor Robbins!"

struct GameState_t
{
	bool initialized;
	
	PDMenuItem* backgroundMenuItem;
	bool backgroundEnabled;
	
	Font_t mainFont;
	Font_t smallFont;
	Font_t gameFont;
	
	SpriteSheet_t testSheet;
	SpriteSheet_t pieSheet;
	
	Texture_t backgroundTexture;
	Texture_t pigTexture;
	
	Sound_t testSound;
	
	v2i testSheetFrame;
	
	v2 pigPos;
	v2 pigVel;
	
	bool followingCrank;
	v2 pigOldVel;
	
	bool isInverted;
	
	r32 holdBTime;
};

#endif //  _GAME_H
