/*
File:   main.cpp
Author: Taylor Robbins
Date:   09\06\2023
Description: 
	** Holds the main entry point for the application
*/

#include "pd_api.h"

#include "version.h"

#include "gylib/gy_defines_check.h"

#define GYLIB_LOOKUP_PRIMES_10
#include "gylib/gy.h"

// +--------------------------------------------------------------+
// |                           setup.c                            |
// +--------------------------------------------------------------+
static void* (*pdrealloc)(void* ptr, size_t size);

#if 0

#if TARGET_PLAYDATE

void* _malloc_r(struct _reent* _REENT, size_t nbytes) { return pdrealloc(NULL,nbytes); }
void* _realloc_r(struct _reent* _REENT, void* ptr, size_t nbytes) { return pdrealloc(ptr,nbytes); }
void _free_r(struct _reent* _REENT, void* ptr ) { if ( ptr != NULL ) pdrealloc(ptr,0); }

#else

void* malloc(size_t nbytes) { return pdrealloc(NULL,nbytes); }
void* realloc(void* ptr, size_t nbytes) { return pdrealloc(ptr,nbytes); }
void  free(void* ptr ) { if ( ptr != NULL ) pdrealloc(ptr,0); }

#endif

#endif

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

#define TEXT_WIDTH 112
#define TEXT_HEIGHT 16
#define DISPLAY_TEXT "Taylor Robbins!"

int x = (400-TEXT_WIDTH)/2;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;

// +--------------------------------------------------------------+
// |                           MainLoop                           |
// +--------------------------------------------------------------+
static int MainLoop(void* userdata)
{
	PlaydateAPI* pd = (PlaydateAPI*)userdata;
	
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText(DISPLAY_TEXT, strlen(DISPLAY_TEXT), kASCIIEncoding, x, y);
	
	x += dx;
	y += dy;
	
	if (x < 0 || x > LCD_COLUMNS - TEXT_WIDTH) { dx = -dx; }
	if (y < 0 || y > LCD_ROWS - TEXT_HEIGHT) { dy = -dy; }
	
	pd->system->drawFPS(0,0);
	
	return 1;
}

// +--------------------------------------------------------------+
// |                        Event Handler                         |
// +--------------------------------------------------------------+
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	switch (event)
	{
		// +==============================+
		// |             Init             |
		// +==============================+
		case kEventInit:
		{
			const char* err;
			font = pd->graphics->loadFont(fontpath, &err);
			if (font == NULL) { pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err); }
			
			pd->system->setUpdateCallback(MainLoop, pd);
		} break;
		
		// +==============================+
		// |           InitLua            |
		// +==============================+
		case kEventInitLua:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |             Lock             |
		// +==============================+
		case kEventLock:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |            Unlock            |
		// +==============================+
		case kEventUnlock:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |            Pause             |
		// +==============================+
		case kEventPause:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |            Resume            |
		// +==============================+
		case kEventResume:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |          Terminate           |
		// +==============================+
		case kEventTerminate:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |          KeyPressed          |
		// +==============================+
		case kEventKeyPressed:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |         KeyReleased          |
		// +==============================+
		case kEventKeyReleased:
		{
			//TODO: Implement me!
		} break;
		
		// +==============================+
		// |           LowPower           |
		// +==============================+
		case kEventLowPower:
		{
			//TODO: Implement me!
		} break;
	}
	
	return 0;
}

EXTERN_C_START
#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandlerShim(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if (event == kEventInit) { pdrealloc = playdate->system->realloc; }
	return eventHandler(playdate, event, arg);
}
EXTERN_C_END
