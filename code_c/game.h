/*
File:   game.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _GAME_H
#define _GAME_H

#define FONT_PATH "/System/Fonts/Asheville-Sans-14-Bold.pft"
// const char* fontpath = "/System/Fonts/Bitmore.pft";

// #define TEXT_WIDTH 112
// #define TEXT_HEIGHT 16
// #define DISPLAY_TEXT "Taylor Robbins!"

struct GameState_t
{
	bool initialized;
	
	LCDFont* font;
	v2i testSheetFrame;
	SpriteSheet_t testSheet;
	
	Texture_t pigTexture;
	v2i pigPos;
	v2i pigVel;
	
	bool followingCrank;
	v2i pigOldVel;
	
	bool isInverted;
};

#endif //  _GAME_H
