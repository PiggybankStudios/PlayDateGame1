/*
File:   game.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds the main game logic (aka GameInitialize and GameUpdate)
*/

// +--------------------------------------------------------------+
// |                           Helpers                            |
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
void DebugConsoleToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(app->debugConsoleMenuItem) != 0);
	if (app->debugConsoleEnabled != newValue)
	{
		app->debugConsoleEnabled = newValue;
		PrintLine_I("Debug Console %s", app->debugConsoleEnabled ? "Enabled" : "Disabled");
	}
}

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
void GameInitialize()
{
	const char* loadFontErrorStr;
	game->font = pd->graphics->loadFont(FONT_PATH, &loadFontErrorStr);
	if (game->font == NULL)
	{
		PrintLine_E("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, FONT_PATH, loadFontErrorStr);
		AssertMsg(false, "Couldn't load font!");
	}
	
	game->testSheet = LoadSpriteSheet(NewStr("Resources/Sheets/test"), 5);
	Assert(game->testSheet.isValid);
	PrintLine_D("testSheet: (%d, %d) frames, each %dx%d", game->testSheet.numFramesX, game->testSheet.numFramesY, game->testSheet.frameSize.width, game->testSheet.frameSize.height);
	
	game->pieSheet = LoadSpriteSheet(NewStr("Resources/Sheets/pie_badge_small"), 6);
	Assert(game->pieSheet.isValid);
	
	game->backgroundTexture = LoadTexture(NewStr("Resources/Textures/background"));
	Assert(game->backgroundTexture.isValid);
	
	game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
	Assert(game->pigTexture.isValid);
	
	app->fpsDisplayMenuItem = pd->system->addCheckmarkMenuItem("FPS", 1, FpsToggledCallback, nullptr);
	NotNull(app->fpsDisplayMenuItem);
	pd->system->setMenuItemValue(app->fpsDisplayMenuItem, app->fpsDisplayEnabled ? 1 : 0);
	
	app->debugConsoleMenuItem = pd->system->addCheckmarkMenuItem("Debug", 1, DebugConsoleToggledCallback, nullptr);
	NotNull(app->debugConsoleMenuItem);
	pd->system->setMenuItemValue(app->debugConsoleMenuItem, app->debugConsoleEnabled ? 1 : 0);
	
	game->pigPos.x = (ScreenSize.width - game->pigTexture.width) / 2.0f;
	game->pigPos.y = (ScreenSize.height - game->pigTexture.height) / 2.0f;
	game->pigVel.x = 1;
	game->pigVel.y = 2;
	
	game->initialized = true;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void GameUpdate()
{
	MemArena_t* scratch = GetScratchArena();
	
	MyStr_t pigEngineText = NewStr("Pig Engine");
	v2i pigEngineTextSize = MeasureText(game->font, pigEngineText);
	v2i totalWidgetSize = NewVec2i(
		MaxI32(game->pigTexture.width, pigEngineTextSize.width),
		game->pigTexture.height + pigEngineTextSize.height
	);
	
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		game->isInverted = !game->isInverted;
		game->testSheetFrame.x++;
		if (game->testSheetFrame.x >= game->testSheet.numFramesX)
		{
			game->testSheetFrame.x = 0;
			game->testSheetFrame.y++;
			if (game->testSheetFrame.y >= game->testSheet.numFramesY)
			{
				game->testSheetFrame.y = 0;
			}
		}
	}
	if (BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		game->pigVel.x += SignOfR32(game->pigVel.x);
		game->pigVel.y += SignOfR32(game->pigVel.y);
	}
	
	if (!IsCrankDocked())
	{
		if (!game->followingCrank)
		{
			game->pigOldVel = game->pigVel;
			game->followingCrank = true;
		}
		game->pigVel = Vec2FromAngle(input->crankAngleRadians + ThreeHalfsPi32, 3);
	}
	else if (game->followingCrank)
	{
		game->pigVel = game->pigOldVel;
		game->followingCrank = false;
	}
	
	game->pigPos += game->pigVel * TimeScale;
	
	if (game->pigPos.x < 0)
	{
		game->pigPos.x = 0;
		game->pigVel.x = AbsR32(game->pigVel.x);
	}
	if (game->pigPos.x > ScreenSize.width - totalWidgetSize.width)
	{
		game->pigPos.x = (r32)(ScreenSize.width - totalWidgetSize.width);
		game->pigVel.x = -AbsR32(game->pigVel.x);
	}
	if (game->pigPos.y < 0)
	{
		game->pigPos.y = 0;
		game->pigVel.y = AbsR32(game->pigVel.y);
	}
	if (game->pigPos.y > ScreenSize.height - totalWidgetSize.height)
	{
		game->pigPos.y = (r32)(ScreenSize.height - totalWidgetSize.height);
		game->pigVel.y = -AbsR32(game->pigVel.y);
	}
	
	// +==============================+
	// |            Render            |
	// +==============================+
	pd->graphics->clear(game->isInverted ? kColorBlack : kColorWhite);
	pd->graphics->setDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	// PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, ScreenSize));
	
	reci pigIconRec = NewReci(Vec2Roundi(game->pigPos), game->pigTexture.size);
	v2i pigEngineTextPos = pigIconRec.topLeft + NewVec2i(0, pigIconRec.height);
	pd->graphics->setFont(game->font);
	PdDrawText(pigEngineText, pigEngineTextPos);
	PdDrawTexturedRec(game->pigTexture, pigIconRec);
	
	PdDrawSheetFrame(game->testSheet, game->testSheetFrame, Vec2i_Zero);
	
	if (IsCrankUndocked())
	{
		r32 crankPercent = input->crankAngleRadians / TwoPi32;
		i32 pieFrameIndex = RoundR32i(24 * (1.0f - crankPercent));
		v2i pieFrame = NewVec2i(pieFrameIndex % game->pieSheet.numFramesX, pieFrameIndex / game->pieSheet.numFramesX);
		v2i pieSize = game->pieSheet.frameSize * 3;
		PdDrawSheetFrame(game->pieSheet, pieFrame, NewReci(ScreenSize.width - pieSize.width, 0, pieSize));
	}
	
	pd->graphics->setDrawMode(kDrawModeNXOR);
	PdDrawTextPrint(NewVec2i(0, 24*1), "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
	PdDrawTextPrint(NewVec2i(0, 24*2), "ElapsedMs: %u (%u)", input->elapsedMsU32, input->realElapsedMsU32);
	PdDrawTextPrint(NewVec2i(0, 24*3), "TimeScale: %.2f (%.2f)", TimeScale, input->realTimeScale);
	PdDrawTextPrint(NewVec2i(0, 24*4), "TimeSinceEpoch: %llu", input->timeSinceEpoch);
	pd->graphics->setDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	if (app->fpsDisplayEnabled)
	{
		pd->system->drawFPS(0,0);
	}
	
	FreeScratchArena(scratch);
}
