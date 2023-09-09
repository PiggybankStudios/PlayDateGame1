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
	
	game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
	Assert(game->pigTexture.isValid);
	
	app->fpsDisplayMenuItem = pd->system->addCheckmarkMenuItem("FPS", 1, FpsToggledCallback, nullptr);
	NotNull(app->fpsDisplayMenuItem);
	pd->system->setMenuItemValue(app->fpsDisplayMenuItem, app->fpsDisplayEnabled ? 1 : 0);
	
	app->debugConsoleMenuItem = pd->system->addCheckmarkMenuItem("Debug", 1, DebugConsoleToggledCallback, nullptr);
	NotNull(app->debugConsoleMenuItem);
	pd->system->setMenuItemValue(app->debugConsoleMenuItem, app->debugConsoleEnabled ? 1 : 0);
	
	game->pigPos.x = (400 - game->pigTexture.width) / 2;
	game->pigPos.y = (240 - game->pigTexture.height) / 2;
	game->pigVel.x = 1;
	game->pigVel.y = 2;
	
	game->initialized = true;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void GameUpdate()
{
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
		game->pigVel.x += SignOfI32(game->pigVel.x);
		game->pigVel.y += SignOfI32(game->pigVel.y);
	}
	
	if (!IsCrankDocked())
	{
		if (!game->followingCrank)
		{
			game->pigOldVel = game->pigVel;
			pd->system->setPeripheralsEnabled(kAccelerometer);
			game->followingCrank = true;
		}
		// game->pigVel = Vec2Roundi(Vec2FromAngle(input->crankAngleRadians + ThreeHalfsPi32, 3));
		game->pigVel.x += RoundR32i(input->accelVec.x);
		game->pigVel.y += RoundR32i(input->accelVec.y);
	}
	else if (game->followingCrank)
	{
		game->pigVel = game->pigOldVel;
		pd->system->setPeripheralsEnabled(kNone);
		game->followingCrank = false;
	}
	
	game->pigPos += game->pigVel;
	
	if (game->pigPos.x < 0 || game->pigPos.x > ScreenSize.width  - totalWidgetSize.width)  { game->pigVel.x = -game->pigVel.x; }
	if (game->pigPos.y < 0 || game->pigPos.y > ScreenSize.height - totalWidgetSize.height) { game->pigVel.y = -game->pigVel.y; }
	
	// +==============================+
	// |            Render            |
	// +==============================+
	pd->graphics->clear(game->isInverted ? kColorBlack : kColorWhite);
	pd->graphics->setDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	reci pigIconRec = NewReci(game->pigPos, game->pigTexture.size);
	v2i pigEngineTextPos = pigIconRec.topLeft + NewVec2i(0, pigIconRec.height);
	pd->graphics->setFont(game->font);
	PdDrawText(pigEngineText, pigEngineTextPos);
	PdDrawTexturedRec(game->pigTexture, pigIconRec);
	
	// r32 crankPercent = input->crankAngleRadians / TwoPi32;
	// v2i pie
	
	PdDrawSheetFrame(game->testSheet, game->testSheetFrame, Vec2i_Zero);
	
	if (app->fpsDisplayEnabled)
	{
		pd->system->drawFPS(0,0);
	}
}
