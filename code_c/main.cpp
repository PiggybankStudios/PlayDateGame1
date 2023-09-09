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
#define GYLIB_SCRATCH_ARENA_AVAILABLE
#define GYLIB_USE_ASSERT_FAILURE_FUNC
#include "gylib/gy.h"

// +--------------------------------------------------------------+
// |                         Header Files                         |
// +--------------------------------------------------------------+
#include "texture.h"
#include "sprite_sheet.h"
#include "input.h"
#include "pd_api_ext.h"
#include "game.h"
#include "main.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
PlaydateAPI* pd = nullptr;
AppState_t* app = nullptr;
MemArena_t* fixedHeap = nullptr;
MemArena_t* mainHeap = nullptr;
AppInput_t* input = nullptr;
GameState_t* game = nullptr;

const v2i ScreenSize = { LCD_COLUMNS, LCD_ROWS };
const v2 ScreenSizef = { (r32)LCD_COLUMNS, (r32)LCD_ROWS };
u32 ProgramTime = 0;
r32 ElapsedMs = 0.0f;
r32 TimeScale = 1.0f;

// +--------------------------------------------------------------+
// |                         Source Files                         |
// +--------------------------------------------------------------+
#include "scratch.cpp"
#include "debug.cpp"
#include "texture.cpp"
#include "sprite_sheet.cpp"
#include "input.cpp"
#include "game.cpp"

// +--------------------------------------------------------------+
// |                     Menu Item Callbacks                      |
// +--------------------------------------------------------------+
void FpsToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(app->fpsDisplayMenuItem) != 0);
	if (app->fpsDisplayEnabled != newValue)
	{
		app->fpsDisplayEnabled = newValue;
		PrintLine_I("FPS Display %s", app->fpsDisplayEnabled ? "Enabled" : "Disabled");
	}
}
void DebugToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(app->debugMenuItem) != 0);
	if (app->debugEnabled != newValue)
	{
		app->debugEnabled = newValue;
		PrintLine_I("Debug %s", app->debugEnabled ? "Enabled" : "Disabled");
	}
}

// +--------------------------------------------------------------+
// |                        Event Handler                         |
// +--------------------------------------------------------------+
int MainUpdateCallback(void* userData)
{
	UpdateAppInput();
	
	if (!app->firstUpdateCalled)
	{
		WriteLine_N("Running...");
		app->programStartTimeSinceEpoch = input->timeSinceEpoch;
		app->firstUpdateCalled = true;
	}
	
	GameUpdate();
	
	return 0;
}

void HandleSystemEvent(PDSystemEvent event, uint32_t arg)
{
	// if (app != nullptr) { PrintLine_D("Event: %s %u", GetPDSystemEventStr(event)); }
	
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
			newApp->initialized = true;
			
			Assert(app == nullptr);
			fixedHeap = &newApp->fixedHeap;
			mainHeap = &newApp->mainHeap;
			app = newApp;
			
			InitScratchArenas(&app->stdHeap, SCRATCH_ARENA_SIZE, SCRATCH_ARENA_MAX_NUM_MARKS);
			
			app->gameStatePntr = AllocStruct(fixedHeap, GameState_t);
			NotNull(app->gameStatePntr);
			game = app->gameStatePntr;
			
			WriteLine_O("+==============================+");
			PrintLine_O("|       %s v%u.%u(%0u)       |", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			WriteLine_O("+==============================+");
			
			app->fpsDisplayMenuItem = pd->system->addCheckmarkMenuItem("FPS", 1, FpsToggledCallback, nullptr);
			NotNull(app->fpsDisplayMenuItem);
			pd->system->setMenuItemValue(app->fpsDisplayMenuItem, app->fpsDisplayEnabled ? 1 : 0);
			
			app->debugMenuItem = pd->system->addCheckmarkMenuItem("Debug", 1, DebugToggledCallback, nullptr);
			NotNull(app->debugMenuItem);
			pd->system->setMenuItemValue(app->debugMenuItem, app->debugEnabled ? 1 : 0);
			
			WriteLine_N("Initializing...");
			input = &app->input;
			InitializeAppInput();
			
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
