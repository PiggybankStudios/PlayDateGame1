/*
File:   main_menu.cpp
Author: Taylor Robbins
Date:   09\09\2023
Description: 
	** Holds the main menu AppState
*/

MainMenuState_t* mmenu = nullptr;

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_MainMenu(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		for (u64 nIndex = 0; nIndex < ArrayCount(mmenu->randomNumbers); nIndex++)
		{
			mmenu->randomNumbers[nIndex] = GetRandU64(&app->random, 0, 1000);
		}
		
		mmenu->initialized = true;
	}
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_MainMenu(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		//TODO: Free things!
		ClearPointer(mmenu);
	}
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_MainMenu()
{
	MemArena_t* scratch = GetScratchArena();
	
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		PushAppState(AppState_Game);
	}
	
	//TODO: Implement me!
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_MainMenu(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	
	pd->graphics->clear(kColorBlack);
	PdSetDrawMode(kDrawModeCopy);
	
	//TODO: Implement me!
	
	if (app->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (app->perfGraph.enabled) { textPos.y += app->perfGraph.mainRec.y + app->perfGraph.mainRec.height + 1; }
		PdBindFont(&app->debugFont);
		i32 stepY = app->debugFont.lineHeight + 1;
		
		u64 numSoundInstances = 0;
		for (u64 iIndex = 0; iIndex < MAX_SOUND_INSTANCES; iIndex++) { if (app->soundPool.instances[iIndex].isPlaying) { numSoundInstances++; } }
		PdDrawTextPrint(textPos, "%llu sound instance%s", numSoundInstances, Plural(numSoundInstances, "s"));
		textPos.y += stepY;
		
		for (u64 nIndex = 0; nIndex < ArrayCount(mmenu->randomNumbers); nIndex++)
		{
			PdDrawTextPrint(textPos, "%llu", mmenu->randomNumbers[nIndex]);
			textPos.y += stepY;
		}
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_MainMenu()
{
	mmenu = (MainMenuState_t*)RegisterAppState(
		AppState_MainMenu,
		sizeof(MainMenuState_t),
		StartAppState_MainMenu,
		StopAppState_MainMenu,
		UpdateAppState_MainMenu,
		RenderAppState_MainMenu
	);
}
