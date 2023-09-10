/*
File:   app_state.h
Author: Taylor Robbins
Date:   09\09\2023
*/

#ifndef _APP_STATE_H
#define _APP_STATE_H

//TODO: Currently isActive and initialized are really the same thing. Do we really want to allow apps to get popped off the stack but stay initialized??

#define FIRST_APP_STATE AppState_MainMenu

#define MAX_NUM_APP_STATE_CHANGE_REQUESTS   4

enum AppState_t
{
	AppState_None = 0,
	AppState_MainMenu,
	AppState_Game,
	AppState_NumStates,
};
const char* GetAppStateStr(AppState_t enumValue)
{
	switch (enumValue)
	{
		case AppState_None:     return "None";
		case AppState_MainMenu: return "MainMenu";
		case AppState_Game:     return "Game";
		default: return "Unknown";
	}
}

#define APP_STATE_START_DEFINITION(functionName) void functionName(bool initialize, AppState_t prevState)
typedef APP_STATE_START_DEFINITION(AppStateStart_f);
#define APP_STATE_STOP_DEFINITION(functionName) void functionName(bool deinitialize, AppState_t nextState)
typedef APP_STATE_STOP_DEFINITION(AppStateStop_f);
#define APP_STATE_UPDATE_DEFINITION(functionName) void functionName()
typedef APP_STATE_UPDATE_DEFINITION(AppStateUpdate_f);
#define APP_STATE_RENDER_DEFINITION(functionName) void functionName(bool isOnTop)
typedef APP_STATE_RENDER_DEFINITION(AppStateRender_f);

struct AppStateInfo_t
{
	u64 dataSize;
	void* dataPntr;
	
	AppStateStart_f*  Start;
	AppStateStop_f*   Stop;
	AppStateUpdate_f* Update;
	AppStateRender_f* Render;
	
	bool initialized;
	bool isActive;
};

enum AppStateChangeType_t
{
	AppStateChangeType_None = 0,
	AppStateChangeType_Push,
	AppStateChangeType_Pop,
	AppStateChangeType_Change,
	AppStateChangeType_NumTypes,
};
const char* GetAppStateChangeTypeStr(AppStateChangeType_t enumValue)
{
	switch (enumValue)
	{
		case AppStateChangeType_None:   return "None";
		case AppStateChangeType_Push:   return "Push";
		case AppStateChangeType_Pop:    return "Pop";
		case AppStateChangeType_Change: return "Change";
		default: return "Unknown";
	}
}

struct AppStateChange_t
{
	AppStateChangeType_t type;
	AppState_t requestingState;
	AppState_t requestedState;
};

struct AppStateStack_t
{
	u64 stackSize;
	AppState_t stack[AppState_NumStates];
	AppStateInfo_t infos[AppState_NumStates];
	
	AppState_t contextAppState; //The AppState who's code is currently running
	u64 numChanges;
	AppStateChange_t changes[MAX_NUM_APP_STATE_CHANGE_REQUESTS];
};

#endif //  _APP_STATE_H
