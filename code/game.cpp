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
void BackgroundToggledCallback(void* userData)
{
	bool newValue = (pd->system->getMenuItemValue(game->backgroundMenuItem) != 0);
	if (game->backgroundEnabled != newValue)
	{
		game->backgroundEnabled = newValue;
		PrintLine_I("Background %s", game->backgroundEnabled ? "Enabled" : "Disabled");
	}
}

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
void GameInitialize()
{
	game->backgroundEnabled = true;
	game->backgroundMenuItem = pd->system->addCheckmarkMenuItem("Bg", 1, BackgroundToggledCallback, nullptr);
	NotNull(game->backgroundMenuItem);
	pd->system->setMenuItemValue(game->backgroundMenuItem, game->backgroundEnabled ? 1 : 0);
	
	const char* loadFontErrorStr;
	
	game->mainFont = pd->graphics->loadFont(MAIN_FONT_PATH, &loadFontErrorStr);
	if (game->mainFont == NULL)
	{
		PrintLine_E("Couldn't load mainFont %s: %s", MAIN_FONT_PATH, loadFontErrorStr);
		AssertMsg(false, "Couldn't load mainFont!");
	}
	
	game->smallFont = pd->graphics->loadFont(SMALL_FONT_PATH, &loadFontErrorStr);
	if (game->smallFont == NULL)
	{
		PrintLine_E("Couldn't load smallFont %s: %s", SMALL_FONT_PATH, loadFontErrorStr);
		AssertMsg(false, "Couldn't load smallFont!");
	}
	
	game->testSheet = LoadSpriteSheet(NewStr("Resources/Sheets/test"), 5);
	Assert(game->testSheet.isValid);
	PrintLine_D("testSheet: (%d, %d) frames, each %dx%d", game->testSheet.numFramesX, game->testSheet.numFramesY, game->testSheet.frameSize.width, game->testSheet.frameSize.height);
	game->testSheetFrame = NewVec2i(game->testSheet.numFramesX-1, game->testSheet.numFramesY-1);
	
	game->pieSheet = LoadSpriteSheet(NewStr("Resources/Sheets/pie_badge_small"), 6);
	Assert(game->pieSheet.isValid);
	
	game->backgroundTexture = LoadTexture(NewStr("Resources/Textures/background"));
	Assert(game->backgroundTexture.isValid);
	
	game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
	Assert(game->pigTexture.isValid);
	
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
	v2i pigEngineTextSize = MeasureText(game->mainFont, pigEngineText);
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
	LCDBitmapDrawMode defaultDrawMode = (game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	pd->graphics->setDrawMode(defaultDrawMode);
	
	if (game->backgroundEnabled)
	{
		PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, ScreenSize));
	}
	
	{
		reci pigIconRec = NewReci(Vec2Roundi(game->pigPos), game->pigTexture.size);
		v2i pigEngineTextPos = pigIconRec.topLeft + NewVec2i(0, pigIconRec.height);
		pd->graphics->setDrawMode(kDrawModeNXOR);
		pd->graphics->setFont(game->mainFont);
		PdDrawText(pigEngineText, pigEngineTextPos);
		pd->graphics->setDrawMode(defaultDrawMode);
		PdDrawTexturedRec(game->pigTexture, pigIconRec);
	}
	
	v2i testFramePos = NewVec2i(
		ScreenSize.width/2 - game->pieSheet.frameSize.width/2,
		ScreenSize.height/2 - game->pieSheet.frameSize.height/2
	);
	PdDrawSheetFrame(game->testSheet, game->testSheetFrame, testFramePos);
	
	if (IsCrankUndocked())
	{
		r32 crankPercent = input->crankAngleRadians / TwoPi32;
		i32 pieFrameIndex = RoundR32i(24 * (1.0f - crankPercent));
		v2i pieFrame = NewVec2i(pieFrameIndex % game->pieSheet.numFramesX, pieFrameIndex / game->pieSheet.numFramesX);
		v2i pieSize = Vec2Roundi(ToVec2(game->pieSheet.frameSize) * LerpR32(1, 5, crankPercent));
		PdDrawSheetFrame(game->pieSheet, pieFrame, NewReci(ScreenSize.width - pieSize.width, 0, pieSize));
	}
	
	if (app->debugEnabled)
	{
		pd->graphics->setDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		pd->graphics->setFont(game->smallFont);
		i32 fontHeight = pd->graphics->getFontHeight(game->smallFont);
		
		PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
		textPos.y += fontHeight;
		
		PdDrawTextPrint(textPos, "ElapsedMs: %u (%u)", input->elapsedMsU32, input->realElapsedMsU32);
		textPos.y += fontHeight;
		
		PdDrawTextPrint(textPos, "TimeScale: %.2f (%.2f)", TimeScale, input->realTimeScale);
		textPos.y += fontHeight;
		
		PdDrawTextPrint(textPos, "TimeSinceEpoch: %llu", input->timeSinceEpoch);
		textPos.y += fontHeight;
		
		pd->graphics->setDrawMode(defaultDrawMode);
	}
	
	if (app->fpsDisplayEnabled)
	{
		pd->system->drawFPS(0,0);
	}
	
	FreeScratchArena(scratch);
}
