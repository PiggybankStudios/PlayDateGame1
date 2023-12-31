/*
File:   game.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds the game AppState
*/

GameState_t* game = nullptr;

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

const char* ComboOptions[] =
{
	"Option1",
	"Option2",
	"Option3",
	"Option4",
	"Test",
};
void ComboMenuItemCallback(void* userData)
{
	i32 newValue = pd->system->getMenuItemValue(game->comboMenuItem);
	if (game->comboValue != newValue)
	{
		game->comboValue = newValue;
		PrintLine_I("Combo %d", game->comboValue);
	}
}

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_Game(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		// game->backgroundEnabled = true;
		// game->backgroundMenuItem = pd->system->addCheckmarkMenuItem("Bg", 1, BackgroundToggledCallback, nullptr);
		// NotNull(game->backgroundMenuItem);
		// pd->system->setMenuItemValue(game->backgroundMenuItem, game->backgroundEnabled ? 1 : 0);
		
		game->comboValue = 1;
		game->comboMenuItem = pd->system->addOptionsMenuItem("Combo", ComboOptions, ArrayCount(ComboOptions), ComboMenuItemCallback, nullptr);
		NotNull(game->comboMenuItem);
		pd->system->setMenuItemValue(game->comboMenuItem, game->comboValue);
		
		game->mainFont = LoadFont(NewStr(MAIN_FONT_PATH));
		Assert(game->mainFont.isValid);
		game->smallFont = LoadFont(NewStr(SMALL_FONT_PATH));
		Assert(game->smallFont.isValid);
		game->gameFont = LoadFont(NewStr(GAME_FONT_PATH));
		Assert(game->gameFont.isValid);
		
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
		
		game->testSound = LoadSound(NewStr("Resources/Sounds/test"));
		Assert(game->testSound.isValid);
		
		game->pigPos.x = (ScreenSize.width - game->pigTexture.width) / 2.0f;
		game->pigPos.y = (ScreenSize.height - game->pigTexture.height) / 2.0f;
		game->pigVel.x = 1;
		game->pigVel.y = 2;
		
		game->initialized = true;
	}
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_Game(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		// pd->system->removeMenuItem(game->backgroundMenuItem);
		pd->system->removeMenuItem(game->comboMenuItem);
		//TODO: Free things!
		ClearPointer(game);
	}
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_Game()
{
	MemArena_t* scratch = GetScratchArena();
	
	MyStr_t pigEngineText = NewStr("Pig Engine");
	v2i pigEngineTextSize = MeasureText(game->mainFont.font, pigEngineText);
	v2i totalWidgetSize = NewVec2i(
		MaxI32(game->pigTexture.width, pigEngineTextSize.width),
		game->pigTexture.height + pigEngineTextSize.height
	);
	
	if (BtnDownRaw(Btn_B))
	{
		game->holdBTime += ElapsedMs;
		if (game->holdBTime >= GAME_RETURN_TO_MENU_HOLD_B_TIME)
		{
			PopAppState();
			game->holdBTime = 0.0f;
		}
	}
	else { game->holdBTime = 0.0f; }
	
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
		
		PlaySound(&game->testSound);
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
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_Game(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	
	pd->graphics->clear(game->isInverted ? kColorBlack : kColorWhite);
	PdSetDrawMode(game->isInverted ? kDrawModeInverted : kDrawModeCopy);
	
	if (game->backgroundEnabled)
	{
		PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, ScreenSize));
	}
	
	{
		MyStr_t pigEngineText = NewStr("Pig Engine");
		reci pigIconRec = NewReci(Vec2Roundi(game->pigPos), game->pigTexture.size);
		v2i pigEngineTextPos = pigIconRec.topLeft + NewVec2i(0, pigIconRec.height);
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdBindFont(&game->mainFont);
		PdDrawText(pigEngineText, pigEngineTextPos);
		pd->graphics->setDrawMode(oldDrawMode);
		PdDrawTexturedRec(game->pigTexture, pigIconRec);
	}
	
	v2i testFramePos = NewVec2i(
		ScreenSize.width/2 - game->testSheet.frameSize.width/2,
		ScreenSize.height/2 - game->testSheet.frameSize.height/2
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
	
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (pig->perfGraph.enabled) { textPos.y += pig->perfGraph.mainRec.y + pig->perfGraph.mainRec.height + 1; }
		PdBindFont(&pig->debugFont);
		i32 stepY = pig->debugFont.lineHeight + 1;
		
		// PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "ElapsedMs: %u (%u)", input->elapsedMsU32, input->realElapsedMsU32);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "TimeScale: %.2f (%.2f)", TimeScale, input->realTimeScale);
		// textPos.y += stepY;
		
		// PdDrawTextPrint(textPos, "TimeSinceEpoch: %llu", input->timeSinceEpoch);
		// textPos.y += stepY;
		
		u64 numSoundInstances = 0;
		for (u64 iIndex = 0; iIndex < MAX_SOUND_INSTANCES; iIndex++) { if (pig->soundPool.instances[iIndex].isPlaying) { numSoundInstances++; } }
		PdDrawTextPrint(textPos, "%llu sound instance%s", numSoundInstances, Plural(numSoundInstances, "s"));
		textPos.y += stepY;
		
		PdDrawTextPrint(textPos, "main: %llu chars Height:%d %s", game->mainFont.numChars, game->mainFont.lineHeight, GetFontCapsStr(game->mainFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "small: %llu chars Height:%d %s", game->smallFont.numChars, game->smallFont.lineHeight, GetFontCapsStr(game->smallFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "debug: %llu chars Height:%d %s", pig->debugFont.numChars, pig->debugFont.lineHeight, GetFontCapsStr(pig->debugFont));
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "game: %llu chars Height:%d %s", game->gameFont.numChars, game->gameFont.lineHeight, GetFontCapsStr(game->gameFont));
		textPos.y += stepY;
		
		for (u8 rangeIndex = 0; rangeIndex < FontRange_NumRanges; rangeIndex++)
		{
			FontRange_t range = FontRangeByIndex(rangeIndex);
			if (FontHasRange(game->gameFont, range, true))
			{
				u8 numCharsInRange = GetNumCharsInFontRange(range);
				for (u8 charIndex = 0; charIndex < numCharsInRange; charIndex++)
				{
					u32 codepoint = GetFontRangeChar(range, charIndex);
					LCDFontPage* fontPage = pd->graphics->getFontPage(game->gameFont.font, codepoint);
					if (fontPage != nullptr)
					{
						LCDBitmap* glyphBitmap = nullptr;
						i32 glyphAdvance = 0;
						LCDFontGlyph* fontGlyph = pd->graphics->getPageGlyph(fontPage, codepoint, &glyphBitmap, &glyphAdvance);
						if (fontGlyph != nullptr && glyphBitmap != nullptr && glyphAdvance > 0)
						{
							v2i glyphBitmapSize = GetBitmapSize(glyphBitmap);
							if (textPos.x + glyphAdvance > ScreenSize.width)
							{
								textPos.x = 1;
								textPos.y += stepY;
							}
							PdDrawTexturedRec(glyphBitmap, glyphBitmapSize, NewReci(textPos, glyphBitmapSize));
							textPos.x += glyphAdvance;
						}
					}
				}
				textPos.x = 1;
				textPos.y += stepY;
			}
		}
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_Game()
{
	game = (GameState_t*)RegisterAppState(
		AppState_Game,
		sizeof(GameState_t),
		StartAppState_Game,
		StopAppState_Game,
		UpdateAppState_Game,
		RenderAppState_Game
	);
}
