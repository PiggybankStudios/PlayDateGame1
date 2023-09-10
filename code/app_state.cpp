/*
File:   app_state.cpp
Author: Taylor Robbins
Date:   09\09\2023
Description: 
	** Holds functions that help us manage AppStateStack_t and call out to AppState functions
*/

AppState_t GetCurrentAppState()
{
	return (app->appStates.stackSize > 0) ? app->appStates.stack[app->appStates.stackSize-1] : AppState_None;
}
bool IsCurrentAppState()
{
	Assert(app->appStates.contextAppState != AppState_None);
	return (app->appStates.contextAppState == GetCurrentAppState());
}
bool IsAppStateInitialized(AppState_t appState)
{
	return app->appStates.infos[appState].initialized;
}
bool IsAppStateActive(AppState_t appState)
{
	return app->appStates.infos[appState].isActive;
}

void StartAppState(AppState_t appState, bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	AppStateInfo_t* info = &app->appStates.infos[appState];
	NotNull(info->Start);
	Assert(initialize == !info->initialized);
	app->appStates.contextAppState = appState;
	PrintLine_D("%s AppState_%s", (initialize ? "Initializing" : "Starting"), GetAppStateStr(appState));
	if (info->Start) { info->Start(initialize, prevState, transitionStr); }
	if (initialize) { info->initialized = true; }
	info->isActive = true;
	app->appStates.contextAppState = AppState_None;
}

void StopAppState(AppState_t appState, bool deinitialize, AppState_t nextState)
{
	AppStateInfo_t* info = &app->appStates.infos[appState];
	NotNull(info->Stop);
	Assert(info->initialized);
	app->appStates.contextAppState = appState;
	PrintLine_D("%s AppState_%s", (deinitialize ? "Deinitializing" : "Stopping"), GetAppStateStr(appState));
	if (info->Stop) { info->Stop(deinitialize, nextState); }
	if (deinitialize) { info->initialized = false; }
	info->isActive = false;
	app->appStates.contextAppState = AppState_None;
}

void UpdateAppState(AppState_t appState)
{
	AppStateInfo_t* info = &app->appStates.infos[appState];
	NotNull(info->Update);
	Assert(info->initialized);
	app->appStates.contextAppState = appState;
	if (info->Update) { info->Update(); }
	app->appStates.contextAppState = AppState_None;
}

void RenderAppState(AppState_t appState, bool isOnTop)
{
	AppStateInfo_t* info = &app->appStates.infos[appState];
	NotNull(info->Render);
	Assert(info->initialized);
	app->appStates.contextAppState = appState;
	if (info->Render) { info->Render(isOnTop); }
	app->appStates.contextAppState = AppState_None;
}

bool IsFullAppState(AppState_t appState)
{
	bool result = true;
	AppStateInfo_t* info = &app->appStates.infos[appState];
	Assert(info->initialized);
	if (info->IsFull != nullptr) { result = info->IsFull(); }
	return result;
}

void StartFirstAppState()
{
	app->appStates.stack[0] = FIRST_APP_STATE;
	app->appStates.stackSize = 1;
	StartAppState(FIRST_APP_STATE, true, AppState_None, MyStr_Empty);
}

void UpdateAndRenderAppStateStack()
{
	for (u64 cIndex = 0; cIndex < app->appStates.numChanges; cIndex++)
	{
		AppStateChange_t* change = &app->appStates.changes[cIndex];
		switch (change->type)
		{
			case AppStateChangeType_Push:
			{
				Assert(app->appStates.stackSize < ArrayCount(app->appStates.stack));
				AppStateInfo_t* requestedInfo = &app->appStates.infos[change->requestedState];
				AppState_t prevState = GetCurrentAppState();
				if (prevState != AppState_None) { StopAppState(prevState, false, change->requestedState); }
				StartAppState(change->requestedState, !requestedInfo->initialized, prevState, change->transitionStr);
				app->appStates.stack[app->appStates.stackSize] = change->requestedState;
				app->appStates.stackSize++;
			} break;
			
			case AppStateChangeType_Pop:
			{
				Assert(app->appStates.stackSize <= ArrayCount(app->appStates.stack));
				AppState_t prevState = GetCurrentAppState();
				Assert(prevState == change->requestingState);
				AppState_t newState = (app->appStates.stackSize > 1 ? app->appStates.stack[app->appStates.stackSize-2] : AppState_None);
				StopAppState(prevState, !change->stayInitialized, newState);
				app->appStates.stackSize--;
				if (newState != AppState_None) { StartAppState(newState, false, prevState, change->transitionStr); }
			} break;
			
			case AppStateChangeType_Change:
			{
				Assert(app->appStates.stackSize <= ArrayCount(app->appStates.stack));
				AppStateInfo_t* requestedInfo = &app->appStates.infos[change->requestedState];
				AppState_t prevState = GetCurrentAppState();
				if (prevState != AppState_None)
				{
					StopAppState(prevState, !change->stayInitialized, change->requestedState);
					app->appStates.stackSize--;
				}
				StartAppState(change->requestedState, !requestedInfo->initialized, prevState, change->transitionStr);
				app->appStates.stack[app->appStates.stackSize] = change->requestedState;
				app->appStates.stackSize++;
			} break;
			
			default: Assert(false); break;
		}
		if (!IsEmptyStr(change->transitionStr))
		{
			FreeString(mainHeap, &change->transitionStr);
		}
	}
	app->appStates.numChanges = 0;
	
	if (app->appStates.stackSize > 0)
	{
		UpdateAppState(app->appStates.stack[app->appStates.stackSize-1]);
	}
	
	u64 firstFullAppStateIndex = 0;
	for (u64 sIndex = app->appStates.stackSize; sIndex > 0; sIndex--)
	{
		if (IsFullAppState(app->appStates.stack[sIndex-1]))
		{
			firstFullAppStateIndex = sIndex-1;
			break;
		}
	}
	
	for (u64 sIndex = firstFullAppStateIndex; sIndex < app->appStates.stackSize; sIndex++)
	{
		RenderAppState(app->appStates.stack[sIndex], (sIndex == app->appStates.stackSize-1));
	}
}

// +--------------------------------------------------------------+
// |                             API                              |
// +--------------------------------------------------------------+
void* RegisterAppState(AppState_t state, u64 dataSize, AppStateStart_f* StartFunc, AppStateStop_f* StopFunc, AppStateUpdate_f* UpdateFunc, AppStateRender_f* RenderFunc, AppStateIsFull_f* IsFullFunc = nullptr)
{
	Assert(state < AppState_NumStates);
	NotNull4(StartFunc, StopFunc, UpdateFunc, RenderFunc);
	AppStateInfo_t* info = &app->appStates.infos[state];
	Assert(info->dataPntr == nullptr);
	ClearPointer(info);
	info->dataSize = dataSize;
	info->dataPntr = AllocMem(fixedHeap, dataSize);
	NotNull(info->dataPntr);
	MyMemSet(info->dataPntr, 0x00, dataSize);
	info->Start = StartFunc;
	info->Stop = StopFunc;
	info->Update = UpdateFunc;
	info->Render = RenderFunc;
	info->IsFull = IsFullFunc;
	return info->dataPntr;
}

void PushAppState(AppState_t newAppState, MyStr_t transitionStr = MyStr_Empty_Const)
{
	Assert(app->appStates.numChanges < MAX_NUM_APP_STATE_CHANGE_REQUESTS);
	Assert(app->appStates.contextAppState != AppState_None);
	Assert(newAppState != AppState_None);
	Assert(!IsAppStateActive(newAppState));
	AppStateChange_t* change = &app->appStates.changes[app->appStates.numChanges];
	ClearPointer(change);
	change->type = AppStateChangeType_Push;
	change->requestingState = app->appStates.contextAppState;
	change->requestedState = newAppState;
	if (!IsEmptyStr(transitionStr)) { change->transitionStr = AllocString(mainHeap, &transitionStr); }
	app->appStates.numChanges++;
}

void PopAppState(MyStr_t transitionStr = MyStr_Empty, bool stayInitialized = false)
{
	Assert(app->appStates.numChanges < MAX_NUM_APP_STATE_CHANGE_REQUESTS);
	Assert(app->appStates.contextAppState != AppState_None);
	AppStateChange_t* change = &app->appStates.changes[app->appStates.numChanges];
	ClearPointer(change);
	change->type = AppStateChangeType_Pop;
	change->requestingState = app->appStates.contextAppState;
	if (!IsEmptyStr(transitionStr)) { change->transitionStr = AllocString(mainHeap, &transitionStr); }
	change->stayInitialized = stayInitialized;
	app->appStates.numChanges++;
}

void ChangeAppState(AppState_t newAppState, MyStr_t transitionStr = MyStr_Empty, bool stayInitialized = false)
{
	Assert(app->appStates.numChanges < MAX_NUM_APP_STATE_CHANGE_REQUESTS);
	Assert(app->appStates.contextAppState != AppState_None);
	Assert(newAppState != AppState_None);
	Assert(!IsAppStateActive(newAppState));
	AppStateChange_t* change = &app->appStates.changes[app->appStates.numChanges];
	ClearPointer(change);
	change->type = AppStateChangeType_Change;
	change->requestingState = app->appStates.contextAppState;
	change->requestedState = newAppState;
	if (!IsEmptyStr(transitionStr)) { change->transitionStr = AllocString(mainHeap, &transitionStr); }
	change->stayInitialized = stayInitialized;
	app->appStates.numChanges++;
}