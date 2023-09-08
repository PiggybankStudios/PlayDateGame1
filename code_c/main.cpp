/*
File:   main.cpp
Author: Taylor Robbins
Date:   09\06\2023
Description: 
	** Holds the main entry point for the application
*/

#include "version.h"

#include "gylib/gy_defines_check.h"

#define GYLIB_LOOKUP_PRIMES_10
#define GYLIB_USE_ASSERT_FAILURE_FUNC
#include "gylib/gy.h"

// +--------------------------------------------------------------+
// |                         Header Files                         |
// +--------------------------------------------------------------+
#include "game.h"
#include "main.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
PlaydateAPI* pd = nullptr;
AppState_t* app = nullptr;
MemArena_t* fixedHeap = nullptr;
MemArena_t* mainHeap = nullptr;
GameState_t* game = nullptr;

const v2i ScreenSize = { LCD_COLUMNS, LCD_ROWS };

// +--------------------------------------------------------------+
// |                         Source Files                         |
// +--------------------------------------------------------------+
#include "pd_api_helpers.cpp"
#include "debug.cpp"
#include "game.cpp"

// +--------------------------------------------------------------+
// |                        Event Handler                         |
// +--------------------------------------------------------------+
int MainUpdateCallback(void* userData)
{
	if (!app->firstUpdateCalled)
	{
		WriteLine_N("Running...");
		app->firstUpdateCalled = true;
	}
	
	GameUpdate();
	
	return 0;
}

void HandleSystemEvent(PDSystemEvent event, uint32_t arg)
{
	if (app != nullptr)
	{
		PrintLine_D("Event: %s %u", GetPDSystemEventStr(event));
	}
	
	switch (event)
	{
		// +==============================+
		// |             Init             |
		// +==============================+
		case kEventInit:
		{
			GyLibDebugOutputFunc = GyLibOutputHandler;
			GyLibDebugPrintFunc = GyLibPrintHandler;
			
			AppState_t* newApp = (AppState_t*)MyMalloc(sizeof(AppState_t));
			NotNull(newApp);
			ClearPointer(newApp);
			newApp->initialized = true;
			InitMemArena_StdHeap(&newApp->stdHeap);
			void* fixedHeapPntr = MyMalloc(FIXED_HEAP_SIZE);
			NotNull(fixedHeapPntr);
			InitMemArena_FixedHeap(&newApp->fixedHeap, FIXED_HEAP_SIZE, fixedHeapPntr);
			InitMemArena_PagedHeapArena(&newApp->mainHeap, MAIN_HEAP_PAGE_SIZE, &newApp->stdHeap, MAIN_HEAP_MAX_NUM_PAGES);
			fixedHeap = &newApp->fixedHeap;
			mainHeap = &newApp->mainHeap;
			newApp->initialized = true;
			Assert(app == nullptr);
			app = newApp;
			
			app->gameStatePntr = AllocStruct(fixedHeap, GameState_t);
			NotNull(app->gameStatePntr);
			game = app->gameStatePntr;
			
			AppInitDebugOutput();
			WriteLine_O("+==============================+");
			PrintLine_O("|       %s v%u.%u(%0u)       |", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			WriteLine_O("+==============================+");
			
			WriteLine_N("Initializing...");
			GameInitialize();
			
			pd->system->setUpdateCallback(MainUpdateCallback, nullptr);
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
}

EXTERN_C_START
#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandlerShim(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	if (event == kEventInit) { pdrealloc = playdate->system->realloc; }
	pd = playdate;
	HandleSystemEvent(event, arg);
	return 0;
}
EXTERN_C_END

// +--------------------------------------------------------------+
// |                      Assertion Callback                      |
// +--------------------------------------------------------------+
void GyLibAssertFailure(const char* filePath, int lineNumber, const char* funcName, const char* expressionStr, const char* messageStr)
{
	if (pd == nullptr) { return; }
	
	if (app != nullptr && app->initialized)
	{
		//TODO: Shorten path to just fileName
		if (messageStr != nullptr && messageStr[0] != '\0')
		{
			pd->system->error("Assertion Failure! %s (Expression: %s) in %s %s:%d", messageStr, expressionStr, funcName, filePath, lineNumber);
		}
		else
		{
			pd->system->error("Assertion Failure! (%s) is not true in %s %s:%d", expressionStr, funcName, filePath, lineNumber);
		}
	}
	else
	{
		MyBreak();
	}
}
